/*** Includes ***/
#include "socket_client.hpp"

#include <algorithm>
#include <cerrno>
#include <cstdint>
#include <memory>
#include <netinet/in.h>
#include <unistd.h>

#include "socket_common.hpp"

/*** Functions ***/

int32_t IPC::socket_client::connect_to_server(std::string address)
{
    int unix_status = 0;
    int32_t status = 0;
    std::string ip {};
    int port = 0;
    struct sockaddr_in server_addr = { 0 };

    // Create a client socket.
    errno = 0;
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        error_print("could not create client socket\n");
        return static_cast<int32_t>(errno);
    }
    m_socket = std::make_unique<socket_guard>(client_socket);

    // Define the server address.
    status = IPC::get_socket_address(address, &server_addr);
    if (0 != status) {
        return status;
    }

    // inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr); // TODO is this
    // needed???

    // Connect to the server.
    errno = 0;
    status = connect(
        m_socket->m_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (0 > status) {
        error_print("connection to server %s:%d failed\n", ip.c_str(), port);
        return EIO;
    }

    return 0;
}

int32_t IPC::socket_client::disconnect()
{
    debug_print("disconnecting socket %d\n", m_socket->m_fd);
    return static_cast<int32_t>(close(m_socket->m_fd));
}

int32_t IPC::socket_client::send_message(IPC::message const& message)
{
    int unix_status = 0;
    auto& msg_data = message.data;
    char data_buf[2048] = { 0 };
    std::size_t data_len = 0;
    std::size_t data_sent = 0;
    IPC::message_metadata msg_metadata { .type = message.type,
        .uid = message.uid,
        .data_len = static_cast<uint32_t>(message.data.size()) };

    memmove(data_buf + data_len, &msg_metadata, sizeof(msg_metadata));
    data_len += sizeof(msg_metadata);

    // Send data of any length.
    while (data_sent < msg_data.size()) {
        std::size_t bytes_to_move = std::min(2048 - data_len, msg_data.size());

        memmove(data_buf + data_len, msg_data.cbegin().base(), bytes_to_move);
        data_len += bytes_to_move;

        errno = 0;
        unix_status = send(m_socket->m_fd, data_buf, data_len, 0);
        if (unix_status < 0) {
            error_print("send failed with %d\n", errno);
            return static_cast<int32_t>(errno);
        }
        data_sent += data_len;
        data_len = 0;
    }

    return 0;
}

int32_t IPC::socket_client::recv_message(IPC::message& message)
{
    int unix_status = 0;
    auto& msg_data = message.data;
    char data_buf[2048] = { 0 };
    std::size_t data_read = 0;
    ssize_t bytes_received = 0;
    IPC::message_metadata msg_metadata {};

    errno = 0;
    bytes_received = recv(m_socket->m_fd, data_buf, sizeof(data_buf), 0);
    if (bytes_received < 0) {
        error_print("recv failed with %d\n", errno);
        return static_cast<int32_t>(errno);
    }
    memmove(&msg_metadata, data_buf, sizeof(msg_metadata));
    msg_data.insert(msg_data.begin(),
        data_buf + sizeof(msg_metadata),
        data_buf + bytes_received);
    data_read += bytes_received - sizeof(msg_metadata);

    while (data_read < msg_metadata.data_len) {
        errno = 0;
        bytes_received = recv(m_socket->m_fd,
            data_buf,
            std::min(sizeof(data_buf), msg_metadata.data_len - data_read),
            0);
        if (bytes_received < 0) {
            error_print("recv failed with %d\n", errno);
            return static_cast<int32_t>(errno);
        }

        msg_data.insert(msg_data.end(), data_buf, data_buf + bytes_received);
        data_read += bytes_received;
    }

    message.type = msg_metadata.type;
    message.uid = msg_metadata.uid;
    return 0;
}