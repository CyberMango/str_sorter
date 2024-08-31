#ifndef _IPC_SERVER_SOCKET_HPP_
#define _IPC_SERVER_SOCKET_HPP_

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <pair>
#include <string>
#include <vector>

#include "ipc_server.hpp"
#include "ipc_socket.hpp"

// TODO make this an abstract class for interface, and implement in a socket_IPC
// TODO break this into a client class and server class.
class IPC_server_socket : public IPC_server {
public:
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
    virutal override int32_t start_server();
    virtual override int32_t wait_for_request();

    // TODO temp fix until i break this into client and server classes.
    virtual override int32_t send_request(IPC_server::message const& request);
    int32_t client_send_request(IPC_server::message const& request);

private:
    struct __attribute__((packed)) message_metadata {
        uint32_t uid;
        uint16_t type;
        uint32_t data_length;
    };

    std::unique_ptr<IPC_socket> m_server_socket;
    std::map<int, int> connections_old;
    std::map<uint32_t, std::unique_ptr<IPC_socket>> connections;
    std::mutex connections_mtx;

    int32_t get_address(struct sockaddr_in& address);
    void add_connection(
        std::unique_ptr<IPC_socket>& new_connection, uint32_t id);
    std::unique_ptr<IPC_socket> pop_connection(uint32_t id);
};

#endif // _IPC_SERVER_SOCKET_HPP_