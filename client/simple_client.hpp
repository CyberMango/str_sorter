#ifndef _SIMPLE_CLIENT_HPP_
#define _SIMPLE_CLIENT_HPP_

#include <cstdint>
#include <iostream>
#include <string>

#include "../ipc/ipc_server_socket.hpp"
#include "../ipc/ipc_socket.hpp"

class simple_client {
public:
    simple_client() = default;
    int32_t send(std::string const& data);
    int32_t recv(uint32_t length, std::string& recevied_data);

private:
    IPC_socket_server ipc;
};

#endif // _SIMPLE_CLIENT_HPP_