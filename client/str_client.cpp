#include "str_client.hpp"

#include <array>
#include <memory>
#include <string>
#include <unistd.h>

#include "../ipc/server.hpp"

int32_t str_client::connect(std::string address)
{
    int32_t status = m_ipc->connect_to_server(address);
    if (0 != status) {
        error_print("connecting to server failed with %d\n", status);
        return status;
    }
    return 0;
}

int32_t str_client::disconnect()
{
    return m_ipc->disconnect();
}

int32_t str_client::send(std::string_view const& data)
{
    uint32_t uid = static_cast<uint32_t>(getuid());
    IPC::message to_send { .type = IPC::message_type::SEND,
        .uid = uid,
        .data = std::vector<uint8_t>(data.cbegin(), data.cend()) };

    int32_t status = m_ipc->send_message(to_send);
    if (0 != status) {
        error_print("sending a message failed\n");
        return status;
    }

    return 0;
}

int32_t str_client::recv(std::size_t length, std::string& recevied_data)
{
    int32_t status = 0;
    uint32_t uid = static_cast<uint32_t>(getuid());
    IPC::message response {};
    IPC::message to_send { .type = IPC::message_type::RECEIVE,
        .uid = uid,
        .data = std::vector<uint8_t>(sizeof(length)) };

    std::copy(reinterpret_cast<uint8_t*>(&length),
        reinterpret_cast<uint8_t*>(&length) + sizeof(length),
        to_send.data.begin());

    debug_print("sending receive message\n");
    status = m_ipc->send_message(to_send);
    if (0 != status) {
        error_print("sending message failed\n");
        return status;
    }

    debug_print("waiting for receive message response\n");
    status = m_ipc->recv_message(response);
    if (0 != status) {
        error_print("receiving message failed\n");
        return status;
    }
    if (IPC::message_type::RESPONSE != response.type) {
        error_print(
            "wrong response type: %hu\n", static_cast<uint16_t>(response.type));
        return EINVAL;
    }

    recevied_data = std::string(response.data.cbegin(), response.data.cend());

    return 0;
}