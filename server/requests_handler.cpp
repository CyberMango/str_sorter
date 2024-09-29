#include "requests_handler.hpp"

#include <thread>

#include "../ipc/server.hpp"
#include "../utility/simple_logger.hpp"

int32_t requests_handler::run()
{
    int32_t status = 0;
    IPC::server::request in_msg = { 0 };
    IPC::server::response out_resp = { 0 };

    // TODO find a nice stop condition.
    while (true) {
        /* Client tells us who he is. In this version we are ok with that.
        There is not enough time to implement security mechanism and such.
        Even if we were to create an established connection with each client
        there would be security holes without a whole system that handles
        the security and not pretending problems.
        */
        status = m_server->wait_for_connection(in_msg);
        if (0 != status) {
            return status;
        }

        debug_print("Creating request thread\n");
        std::thread request_thread(
            &requests_handler::handle_request, this, in_msg);
        debug_print("Waiting on request thread\n");
        request_thread.join();
    }
}

int32_t requests_handler::handle_request(IPC::server::request in_msg)
{
    int32_t status = 0;
    IPC::server::response out_resp = { 0 };

    if (IPC::server::request_type::SEND == in_msg.type) {
        status = store_data(in_msg.uid, in_msg.data);
        if (0 != status) {
            return status;
        }
        // Client wants to receive data.
    } else if (IPC::server::request_type::RECEIVE == in_msg.type) {
        errno = 0;
        char* end = nullptr;
        uint32_t data_len = static_cast<uint32_t>(
            std::strtoll(in_msg.data.c_str(), &end, 10));
        /* TODOO dont actually stop the server due to this error on
        release. you need to print an error message and only stop on
        unrecoverable errors. here and everywhere you are the one
        generating the error.
        */
        if (0 != errno) {
            error_print(
                "Invalid data receive request %s\n", in_msg.data.c_str());
            // TODO return a better error code maybe.
            return ERANGE;
        }
        status = fetch_data(in_msg.uid, data_len, out_resp.data);
        if (0 != status) {
            return status;
        }
        out_resp.id = in_msg.id;
        out_resp.type = in_msg.type;

        status = m_server->send_response(out_resp);
        if (0 != status) {
            return status;
        }
    } else {
        // TODOO
        return EINVAL;
    }

    return 0;
}

// TODO
int32_t requests_handler::store_data(uint32_t uid, std::string data)
{
    return 0;
}

// TODO
int32_t requests_handler::fetch_data(
    uint32_t uid, uint32_t data_len, std::string& data)
{
    data = std::string(data_len, 'a');
    return 0;
}