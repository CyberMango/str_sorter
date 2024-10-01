#ifndef _SIMPLE_CLIENT_HPP_
#define _SIMPLE_CLIENT_HPP_

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

#include "../ipc/server.hpp"
#include "../ipc/socket_client.hpp"

class str_client {
public:
    str_client() = default;
    int32_t connect(std::string address);
    int32_t disconnect();
    int32_t send(std::string_view const& data);
    int32_t recv(std::size_t length, std::string& recevied_data);

private:
    std::unique_ptr<IPC::client> ipc;
};

#endif // _SIMPLE_CLIENT_HPP_