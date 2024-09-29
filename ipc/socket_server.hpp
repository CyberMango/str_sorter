#ifndef _IPC_SOCKET_SERVER_HPP_
#define _IPC_SOCKET_SERVER_HPP_

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>

#include "../utility/simple_logger.hpp"
#include "server.hpp"
#include "socket_common.hpp"

namespace IPC {

class socket_server : public IPC::server {
public:
    socket_server():
        m_socket { nullptr }
    {
    }
    ~socket_server() override
    {
        // TODO close all sockets.
    }
    virtual int32_t start_server() override;
    virtual int32_t wait_for_connection(
        std::unique_ptr<IPC::client>& connection) override;

private:
    std::unique_ptr<IPC::socket_guard> m_socket;

    void get_address(struct sockaddr_in& address);
    void add_connection(
        std::unique_ptr<IPC::socket_guard>& new_connection, uint32_t id);
    std::unique_ptr<IPC::socket_guard> pop_connection(uint32_t id);
};

} // namepsace IPC

#endif // _IPC_SOCKET_SERVER_HPP_