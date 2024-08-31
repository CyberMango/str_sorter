#include "simple_client.hpp"

#include <string>
#include <unistd.h>

#include "../ipc/ipc_server.hpp"

int32_t simple_client::send(std::string_view const& data)
{
    uint32_t uid = static_cast<uint32_t>(getuid());
    IPC_server::message to_send { .uid = uid,
        .type = IPC_server::request_type::SEND,
        .data = std::string(data) };

    ipc.send_request(to_send);

    return 0;
}

int32_t simple_client::recv(uint32_t length, std::string& recevied_data)
{
    uint32_t uid = static_cast<uint32_t>(getuid());
    IPC_server::message to_send { .uid = uid,
        .type = IPC_server::request_type::RECEIVE,
        .data = std::to_string(length) };

    auto resp = ipc.send_request(to_send);

    recevied_data = resp.data;

    return 0;
}