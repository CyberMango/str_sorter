/*** Includes ***/
#include "socket_client.hpp"

#include <algorithm>
#include <array>
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
    struct sockaddr_in server_addr = {};

    // Create a client socket.
    errno = 0;
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        status = static_cast<int32_t>(errno);
        error_print("could not create client socket. errno=%d\n", status);
        return status;
    }
    m_socket = std::make_unique<socket_guard>(client_socket);

    // Connect to the server.
    status = IPC::get_socket_address(address, &server_addr);
    if (0 != status) {
        return status;
    }

    errno = 0;
    unix_status = connect(
        m_socket->m_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (0 > unix_status) {
        status = static_cast<int32_t>(errno);
        error_print("connection to server %s:%d failed. errno=%d\n",
            ip.c_str(),
            port,
            status);
        return status;
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
    int32_t status = 0;
    auto& msg_data = message.data;
    char data_buf[2048] = { 0 };
    std::size_t data_len = 0;
    std::size_t data_sent = 0;
    IPC::message_metadata msg_metadata { .type = message.type,
        .uid = message.uid,
        .data_len = message.data.size() };

    memmove(data_buf + data_len, &msg_metadata, sizeof(msg_metadata));
    data_len += sizeof(msg_metadata);

    auto data_iter = msg_data.cbegin();

    // Send data of any length.
    while (data_sent < msg_data.size()) {
        // TODO move the whole data reading to inner function.
        std::size_t bytes_to_move
            = std::min(2048 - data_len, msg_data.size() - data_sent);

        memmove(data_buf + data_len, data_iter.base(), bytes_to_move);
        data_len += bytes_to_move;
        data_iter += bytes_to_move;

        errno = 0;
        unix_status = send(m_socket->m_fd, data_buf, data_len, 0);
        if (unix_status < 0) {
            status = static_cast<int32_t>(errno);
            error_print("send failed. errno=%d\n", errno);
            return status;
        }
        data_sent += data_len;
        data_len = 0;
    }

    return 0;
}

int32_t IPC::socket_client::recv_message(IPC::message& message)
{
    int32_t status = 0;
    IPC::message_metadata msg_metadata {
        .type = IPC::message_type::INVALID, .uid = 0, .data_len = 0
    };

    // First chunk contains both metadata and message data.
    status = receive_first_data_chunk(msg_metadata, message.data);
    if (0 != status) {
        return status;
    }
    message.type = msg_metadata.type;
    message.uid = msg_metadata.uid;

    // If client has disconnected.
    if (message_type::DISCONNECT == message.type) {
        return 0;
    }

    // Read all the data that was supposed to be sent.
    while (message.data.size() < msg_metadata.data_len) {
        status = receive_data_chunk(message);
        if (0 != status) {
            return status;
        }
    }

    return 0;
}

int32_t IPC::socket_client::receive_first_data_chunk(
    IPC::message_metadata& msg_metadata, std::vector<uint8_t>& msg_data)
{
    int32_t status = 0;
    std::array<uint8_t, 1024> data_buf = { 0 };
    ssize_t bytes_received = 0;

    errno = 0;
    bytes_received = recv(m_socket->m_fd, data_buf.data(), data_buf.size(), 0);
    if (bytes_received < 0) {
        status = static_cast<int32_t>(errno);
        error_print("recv failed. errno=%d\n", status);
        return status;
    }
    if (0 == bytes_received) {
        msg_metadata.type = message_type::DISCONNECT;
        return 0;
    }
    if (bytes_received < static_cast<ssize_t>(sizeof(msg_metadata))) {
        error_print("received less bytes than possible. Got %ld, expected at "
                    "least %lu\n",
            bytes_received,
            sizeof(msg_metadata));
        return EIO;
    }

    memmove(&msg_metadata, data_buf.data(), sizeof(msg_metadata));
    msg_data.insert(msg_data.begin(),
        data_buf.begin() + sizeof(msg_metadata),
        data_buf.begin() + bytes_received);

    return 0;
}

int32_t IPC::socket_client::receive_data_chunk(IPC::message& message)
{
    int32_t status = 0;
    std::array<uint8_t, 1024> data_buf = { 0 };
    ssize_t bytes_received = 0;

    errno = 0;
    bytes_received = recv(m_socket->m_fd, data_buf.data(), data_buf.size(), 0);
    if (bytes_received < 0) {
        status = static_cast<int32_t>(errno);
        error_print("recv failed. errno=%d\n", status);
        return status;
    }
    if (0 == bytes_received) {
        error_print("client disconnected abruptly\n");
        message.type = message_type::DISCONNECT;
        return EIO;
    }

    message.data.insert(message.data.end(),
        data_buf.begin(),
        data_buf.begin() + bytes_received);

    return 0;
}