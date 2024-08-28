/*** Includes ***/

#include "ipc.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/*** Constants ***/
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT (1234)
#define MAX_CONNECTIONS (10)

/*** Functions ***/
void IPC::get_address(struct sockaddr_in& address)
{
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(SERVER_IP);
    address.sin_port = htons(SERVER_PORT);
}

int32_t IPC::start_server()
{
    int status = 0;
    struct sockaddr_in address = { 0 };

    // Create TCP socket
    // TODO encapsulate in a class for auto close on destruction.
    m_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (0 == m_server_fd) {
        // TODO create a better macro for this with file name and lines. better
        // error message too.
        perror("Socker creation failed\n");
        return EIO;
    }

    get_address(address);
    status = bind(
        m_server_fd, static_cast<struct sockaddr*>(&address), sizeof(address));
    if (0 > status) {
        perror("Binding on address failed\n");
        close(m_server_fd);
        return EIO;
    }

    // Listen for incoming connections
    status = listen(m_server_fd, MAX_CONNECTIONS);
    if (0 > status) {
        perror("listening failed\n");
        close(m_server_fd);
        return EIO;
    }

    std::cout << "Server is listening on " << MAX_CONNECTIONS << ":"
              << SERVER_PORT << std::endl;

    // Cleanup
    close(m_server_fd);
    return 0;
}
int32_t IPC::wait_for_request(IPC::message& request)
{
    int new_connection = 0;
    struct sockaddr_in address = { 0 };
    int addrlen = sizeof(address);
    struct message_metadata metadata = { 0 };

    get_address(address);
    // Wait for a connection request
    // TODO encapsulate in a class for auto cleanup.
    new_connection = accept(m_server_fd, (struct sockaddr*)&address,
        static_cast<socklen_t*>(&addrlen));
    if (new_connection < 0) {
        // TODO
        perror("Accept failed");
        return EIO;
    }

    std::cout << "debug: Connection accepted" << std::endl;

    // TODOO for simplicity assuming messages smaller than this. afterwards
    // receive the whole message according to the limits we set in the protocol
    // (uint32_t).
    // Also assuming well formed messages and all with no \0. dont have time for
    // checks.
    uint8_t msg_buf[2048] = { 0 };
    char data_buf[2048 - sizeof(message_metadata) + 1];
    // TODO set the socket to non blocking and use a timeout with fcntl.
    ssize_t bytes_received = recv(new_socket, msg_buf, sizeof(msg_buf) - 1, 0);
    if (-1 == bytes_received) {
        perror("error receiving message");
        close(new_connection);
        return EIO;
    }

    // TODO verify reasonable msg length.
    std::memmove(&metadata, msg_buf, sizeof(metadata));
    std::memmove(
        data_buf, msg_buf + sizeof(message_metadata), metadata.data_length);
    data_buf[metadata.data_length] = '\0';

    request.data = std::string(data_buf);
    request.uid = metadata.uid;
    request.type = metadata.type;

    // Save receive connections so that we can answer them.
    if (static_cast<uint16_t>(IPC::request_type::RECEIVE) == request.type) {
        request.id = add_connection(new_connection);
    }

    return 0;
}

int32_t IPC::server_send_request(IPC::message const& request)
{
    message_metadata metadata = { 0 };
    // TODO again, just quick way to send messages.
    char data_buf[2048];
    int client_socket = 0;

    metadata.type = request.type;
    metadata.data_length = std::len(request.data);
    metadata.uid = request.uid;
    client_socket = pop_connection(metadata.uid, request.id);

    memmove(data_buf, metadata, sizeof(metadata));
    memmove(data_buf + sizeof(metadata), request.data.c_str(),
        metadata.data_length);

    send(client_socket, data_buf, sizeof(metadata) + metadata.data_length, 0);

    return 0;
}

int32_t IPC::client_send_request(IPC::message const& request)
{
    // TODO
}