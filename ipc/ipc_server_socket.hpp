#ifndef _IPC_SERVER_SOCKET_HPP_
#define _IPC_SERVER_SOCKET_HPP_

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>

#include "../utility/simple_logger.hpp"
#include "ipc_server.hpp"

// TODO make this an abstract class for interface, and implement in a socket_IPC
// TODO break this into a client class and server class.
class IPC_server_socket : public IPC_server {
public:
    IPC_server_socket():
        m_server_socket { nullptr },
        connections_old {},
        connections_mtx {}
    {
    }
    ~IPC_server_socket()
    {
        // TODO close all sockets.
    }
    virtual int32_t wait_for_request(IPC_server::message& request) override;
    virtual int32_t start_server() override;
    virtual int32_t send_response(IPC_server::response const& to_send) override;

private:
    class IPC_socket {
    public:
        IPC_socket(int socket_fd = 0):
            m_fd(socket_fd)
        {
        }
        ~IPC_socket()
        {
            debug_print("closing socket %d\n", static_cast<int>(m_fd));
            close(m_fd);
        }

        int m_fd;
    };

    std::unique_ptr<IPC_socket> m_server_socket;
    std::map<int, int> connections_old;
    std::map<uint32_t, std::unique_ptr<IPC_socket>> connections;
    std::mutex connections_mtx;

    void get_address(struct sockaddr_in& address);
    void add_connection(
        std::unique_ptr<IPC_socket>& new_connection, uint32_t id);
    std::unique_ptr<IPC_socket> pop_connection(uint32_t id);
};

#endif // _IPC_SERVER_SOCKET_HPP_