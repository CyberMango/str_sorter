#include "sort_server.hpp"

#include <chrono>
#include <cstring>
#include <future>
#include <memory>
#include <string>
#include <thread>

#include "../ipc/client.hpp"
#include "../ipc/server.hpp"
#include "../ipc/socket/socket_server.hpp"

/*** Functions ***/

sort_server::sort_server(std::string address):
    m_routines {}
{
    std::unique_ptr<IPC::server> server
        = std::make_unique<IPC::socket_server>();

    m_connections
        = std::make_unique<connections_handler>(std::move(server), address);
    m_data = std::make_unique<data_handler>();
}

sort_server::~sort_server()
{
    for (auto& routine : m_routines) {
        (void)routine.get();
    }
}

int32_t sort_server::run()
{
    int32_t status = 0;

    // TODO create a stop condition.
    while (true) {
        std::unique_ptr<IPC::client> connection = nullptr;

        status = m_connections->wait_for_connection(connection);
        if (0 != status) {
            return status;
        }

        debug_print("invoking a client routine\n");
        // Using future to be able to attempt join without blocking.
        std::future<void> client_thread = std::async(std::launch::async,
            &sort_server::client_routine,
            this,
            std::move(connection));
        m_routines.push_back(std::move(client_thread));

        for (auto routine = m_routines.begin(); routine != m_routines.end();) {
            if (std::future_status::ready
                == routine->wait_for(std::chrono::seconds(0))) {
                // Calling get is equivalent to join in regular threads.
                (void)routine->get();
                routine = m_routines.erase(routine);
            } else {
                ++routine;
            }
        }
    }
}

void sort_server::client_routine(std::unique_ptr<IPC::client> connection)
{
    int32_t status = 0;
    IPC::message in_msg {
        .type = IPC::message_type::INVALID, .uid = 0, .data {}
    };

    while (IPC::message_type::DISCONNECT != in_msg.type) {
        status = connection->recv_message(in_msg);
        if (0 != status) {
            error_print("error receiving message: %d\n", status);
            continue;
        }
        debug_print("received message of type %hu\n",
            static_cast<uint16_t>(in_msg.type));

        status = handle_message(in_msg, connection);
        if (0 != status) {
            error_print("error handling message: %d\n", status);
            continue;
        }
    }

    debug_print("client disconnected from server\n");
}

int32_t sort_server::handle_message(
    IPC::message& in_msg, std::unique_ptr<IPC::client>& connection)
{
    int32_t status = 0;
    std::string data {};

    switch (in_msg.type) {
    case IPC::message_type::DISCONNECT:
        return 0;
    case IPC::message_type::SEND:
        data = std::string { in_msg.data.cbegin(), in_msg.data.cend() };
        status = m_data->store_data(in_msg.uid, data);
        if (0 != status) {
            return status;
        }
        break;
    case IPC::message_type::RECEIVE:
        status = handle_receive_message(in_msg, connection);
        if (0 != status) {
            return status;
        }
        break;
    default:
        error_print("unhandled message type: %hu\n",
            static_cast<uint16_t>(in_msg.type));
        return EINVAL;
    }

    return 0;
}

int32_t sort_server::handle_receive_message(
    IPC::message& in_msg, std::unique_ptr<IPC::client>& connection)
{
    int32_t status = 0;
    std::string data {};
    std::size_t length = 0;
    IPC::message out_msg {
        .type = IPC::message_type::RESPONSE, .uid = 0, .data {}
    };

    std::copy(in_msg.data.cbegin(),
        in_msg.data.cend(),
        reinterpret_cast<uint8_t*>(&length));

    status = m_data->fetch_data(in_msg.uid, length, data);
    if (0 != status) {
        error_print("fetching data failed: %d\n", status);
        return status;
    }

    out_msg.data = std::vector<uint8_t>(data.begin(), data.end());
    status = connection->send_message(out_msg);
    if (0 != status) {
        error_print("sending response failed: %d\n", status);
        return status;
    }

    return 0;
}