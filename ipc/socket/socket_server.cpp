/*** Includes ***/

#include "socket_server.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../utility/simple_logger.hpp"
#include "../server.hpp"
#include "socket_client.hpp"
#include "socket_common.hpp"

/*** Functions ***/

int32_t IPC::socket_server::start_server(std::string address)
{
    int unix_status = 0;
    int32_t status = 0;
    int server_fd = 0;

    // Create a TCP socket.
    errno = 0;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (0 == server_fd) {
        status = static_cast<int32_t>(errno);
        error_print("Socket creation failed. errno=%d\n", status);
        return status;
    }
    m_socket = std::make_unique<socket_guard>(server_fd);

    // Bind the server to the given address.
    m_socket_address = std::make_unique<struct sockaddr_in>();
    status = IPC::get_socket_address(address, m_socket_address.get());
    if (0 != status) {
        return status;
    }

    errno = 0;
    unix_status = bind(m_socket->m_fd,
        reinterpret_cast<sockaddr*>(m_socket_address.get()),
        sizeof(*m_socket_address));
    if (0 > unix_status) {
        status = static_cast<int32_t>(errno);
        error_print("Binding on address failed. errno=%d\n", errno);
        return status;
    }

    // Listen for incoming connections.
    errno = 0;
    unix_status = listen(m_socket->m_fd, MAX_CONNECTIONS);
    if (0 > unix_status) {
        status = static_cast<int32_t>(errno);
        error_print("server listening failed. errno=%d\n", errno);
        return status;
    }

    debug_print(
        "Server is listening on socket %d, address %s for max %d connections\n",
        m_socket->m_fd,
        address.c_str(),
        MAX_CONNECTIONS);

    return 0;
}

int32_t IPC::socket_server::wait_for_connection(
    std::unique_ptr<IPC::client>& connection)
{
    int connection_fd = 0;
    int addrlen = sizeof(*m_socket_address);

    debug_print("waiting for a connection\n");
    connection_fd = accept(m_socket->m_fd,
        reinterpret_cast<struct sockaddr*>(m_socket_address.get()),
        (socklen_t*)(&addrlen));
    if (connection_fd < 0) {
        error_print("accept failed\n");
        return EIO;
    }
    connection = std::make_unique<IPC::socket_client>(connection_fd);
    debug_print("connection accepted on socket %d\n", connection_fd);

    return 0;
}