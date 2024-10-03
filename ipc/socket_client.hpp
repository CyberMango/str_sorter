#ifndef _IPC_SOCKET_CLIENT_HPP_
#define _IPC_SOCKET_CLIENT_HPP_

#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <sys/socket.h>
#include <unistd.h>

#include "../utility/simple_logger.hpp"
#include "client.hpp"
#include "server.hpp"
#include "socket_common.hpp"

namespace IPC {

class socket_client : public IPC::client {
public:
    socket_client():
        m_socket(nullptr)
    {
    }

    socket_client(int socket_fd)
    {
        m_socket = std::make_unique<socket_guard>(socket_fd);
    }

    ~socket_client()
    {
        // TODO disconnect i guess?
    }

    virtual int32_t connect_to_server(std::string address) override;
    virtual int32_t disconnect() override;
    virtual int32_t send_message(IPC::message const& message) override;
    virtual int32_t recv_message(IPC::message& message) override;

private:
    std::unique_ptr<socket_guard> m_socket;
};

} // namespace IPC

#endif // _IPC_SOCKET_CLIENT_HPP_