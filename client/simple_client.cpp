#include "simple_client.hpp"

#include <unistd.h>

#include "../ipc/ipc.hpp"
#include "../ipc/ipc_socket.hpp"
#include "../server/requests_handler.hpp"

int32_t simple_client::send(std::string const& data)
{
    uint32_t uid = static_cast<uint32_t>(getuid());
    IPC_socket::message to_send { .uid = uid, // TODOO not the type anymore
        .type = static_cast<uint16_t>(IPC::request_type::SEND),
        .data = data };

    return ipc.send_request(to_send);
}

int32_t simple_client::recv(uint32_t length, std::string& recevied_data)
{
    return 0;
}