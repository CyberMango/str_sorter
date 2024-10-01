#include "str_client.hpp"

#include <string>
#include <unistd.h>

#include "../ipc/server.hpp"

int32_t str_client::connect(std::string address)
{
    int32_t status = ipc->connect_to_server(address);
    if (0 != status) {
        error_print("connecting to server failed with %d\n", status);
        return status;
    }
    return 0;
}

int32_t str_client::disconnect()
{
    return ipc->disconnect();
}

int32_t str_client::send(std::string_view const& data)
{
    uint32_t uid = static_cast<uint32_t>(getuid());
    IPC::message to_send { .type = IPC::message_type::SEND,
        .uid = uid,
        .data = std::vector<uint8_t>(data.cbegin(), data.cend()) };

    int32_t status = ipc->send_message(to_send);
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
    std::string str_length = std::to_string(length);
    IPC::message response {};
    IPC::message to_send { .type = IPC::message_type::RECEIVE,
        .uid = uid,
        .data = std::vector<uint8_t>(str_length.cbegin(), str_length.cend()) };

    status = ipc->send_message(to_send);
    if (0 != status) {
        error_print("sending message failed\n");
        return status;
    }

    status = ipc->recv_message(response);
    if (0 != status) {
        error_print("receiving message failed\n");
        return status;
    }

    recevied_data = std::string(response.data.cbegin(), response.data.cend());

    return 0;
}