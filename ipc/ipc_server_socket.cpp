/*** Includes ***/

#include "ipc_server_socket.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../utility/simple_logger.hpp"
#include "ipc_server.hpp"

/*** Functions ***/

void IPC_server_socket::get_address(struct sockaddr_in& address)
{
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(SERVER_IP);
    address.sin_port = htons(SERVER_PORT);
}

void IPC_server_socket::add_connection(
    std::unique_ptr<IPC_server_socket::IPC_socket>& new_connection, uint32_t id)
{
    std::lock_guard<std::mutex> lock(connections_mtx);

    connections[id] = std::move(new_connection);
}

std::unique_ptr<IPC_server_socket::IPC_socket>
IPC_server_socket::pop_connection(uint32_t id)
{
    std::lock_guard<std::mutex> lock(connections_mtx);

    auto iter = connections.find(id);
    // connection not found.
    if (connections.end() == iter) {
        return nullptr;
    }
    auto connection = std::move(iter->second);
    connections.erase(iter);

    // TODO verify no need move
    return connection;
}

int32_t IPC_server_socket::start_server()
{
    int status = 0;
    int server_fd = 0;
    struct sockaddr_in address = { 0 };

    // Create a TCP socket.
    errno = 0;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (0 == server_fd) {
        error_print("Socket creation failed\n");
        return static_cast<uint32_t>(errno);
    }
    m_server_socket = std::make_unique<IPC_socket>(server_fd);

    // Bind the server on localhost for now.
    get_address(address);
    errno = 0;
    status
        = bind(m_server_socket->m_fd, (sockaddr*)(&address), sizeof(address));
    if (0 > status) {
        error_print("Binding on address failed\n");
        return static_cast<uint32_t>(errno);
    }

    // Listen for incoming connections.
    errno = 0;
    status = listen(m_server_socket->m_fd, MAX_CONNECTIONS);
    if (0 > status) {
        error_print("server listening failed\n");
        return static_cast<uint32_t>(errno);
    }

    debug_print("Server is listening on port %d for %d\n",
        SERVER_PORT,
        MAX_CONNECTIONS);

    return 0;
}

int32_t IPC_server_socket::wait_for_request(IPC_server::message& request)
{
    int connection_fd = 0;
    struct sockaddr_in address = { 0 };
    int addrlen = sizeof(address);
    struct message_metadata metadata = { 0 };
    char recv_buf[2048] = { 0 };

    get_address(address);
    debug_print("Waiting for request\n");
    connection_fd = accept(m_server_socket->m_fd,
        (struct sockaddr*)&address,
        (socklen_t*)(&addrlen));
    if (connection_fd < 0) {
        error_print("Accept failed");
        return EIO;
    }
    auto connection_socket = std::make_unique<IPC_socket>(connection_fd);
    debug_print("Connection accepted on socket %d\n", connection_fd);

    // Wait for a new request's metadata.
    // TODO assuming well formed messages with no \0 or errors to save dev time.
    ssize_t bytes_received
        = recv(connection_socket->m_fd, recv_buf, sizeof(message_metadata), 0);
    if (-1 == bytes_received) {
        error_print("error receiving message metadata");
        return EIO;
    }
    std::memmove(&metadata, recv_buf, sizeof(metadata));

    // Receive the data of the request.
    request.data = std::string {};
    while (request.data.length() < metadata.data_length) {
        bytes_received = recv(connection_socket->m_fd, recv_buf, 2048 - 1, 0);
        if (-1 == bytes_received) {
            error_print("error receiving message data");
            return EIO;
        }
        recv_buf[bytes_received] = '\0';
        request.data += recv_buf;
    }

    request.id = static_cast<uint32_t>(connection_fd);
    request.uid = metadata.uid;
    request.type = metadata.type;

    // Save received connections so that we can answer them.
    if (IPC_server::request_type::RECEIVE == request.type) {
        add_connection(connection_socket, request.id);
    }

    return 0;
}

int32_t IPC_server_socket::send_response(IPC_server::response const& to_send)
{
    message_metadata metadata = { 0 };
    // TODO again, just quick way to send messages. make it better.
    char data_buf[2048];
    std::unique_ptr<IPC_socket> client_socket = nullptr;

    client_socket = std::move(pop_connection(to_send.id));
    if (nullptr == client_socket) {
        error_print("no such connection to respond %d\n", to_send.id);
        return EINVAL;
    }

    // TODO the uid is a filler coz i didnt want to make a response metadata
    // struct.
    metadata.uid = 0;
    metadata.type = to_send.type;
    metadata.data_length = to_send.data.length();

    memmove(data_buf, &metadata, sizeof(metadata));
    memmove(data_buf + sizeof(metadata),
        to_send.data.c_str(),
        metadata.data_length);

    send(client_socket->m_fd,
        data_buf,
        sizeof(metadata) + metadata.data_length,
        0);

    return 0;
}