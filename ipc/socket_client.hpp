#ifndef _IPC_CLIENT_SOCKET_HPP_
#define _IPC_CLIENT_SOCKET_HPP_

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

    virtual int32_t connect_to_server(std::string address) override;
    virtual int32_t disconnect() override;
    virtual int32_t send_message(IPC::message const& message) override;
    virtual int32_t recv_message(IPC::message& message) override;

private:
    std::unique_ptr<socket_guard> m_socket;

    int32_t analyze_address(std::string address, std::string& ip, int& port);
};

// class client_socket_old : public IPC::client {
// public:
//     IPC::message send_request(IPC::server::request const& request)
//     {
//         char data_buf[2048] = { 0 };
//         IPC::server::request_metadata metadata_out = { 0 };
//         IPC::server::request_metadata metadata_in = { 0 };
//         IPC::server::response resp = { 0 };

//         int sock = socket(AF_INET, SOCK_STREAM, 0);
//         if (sock == -1) {
//             std::cerr << "Could not create socket" << std::endl;
//             return resp;
//         }

//         // Define server address
//         sockaddr_in server_adr;
//         server_adr.sin_addr.s_addr = inet_addr(SERVER_IP);
//         server_adr.sin_family = AF_INET;
//         server_adr.sin_port = htons(SERVER_PORT);

//         // Connect to remote server
//         if (connect(sock, (struct sockaddr*)&server_adr, sizeof(server_adr))
//             < 0) {
//             std::cerr << "Connection failed" << std::endl;
//             return resp;
//         }

//         std::cout << "Connected to server" << std::endl;

//         // Receive a reply from the server
//         char server_reply[2000];
//         ssize_t reply_len
//             = recv(sock, server_reply, sizeof(server_reply) - 1, 0);
//         if (reply_len < 0) {
//             std::cerr << "Receive failed" << std::endl;
//             return resp;
//         }

//         // Null-terminate the received data
//         server_reply[reply_len] = '\0';

//         std::cout << "Server reply: " << server_reply << std::endl;

//         memmove(&metadata_in, server_reply, sizeof(metadata_in));

//         resp.id = 0;
//         resp.type = metadata_in.type;
//         resp.data = server_reply + sizeof(metadata_in);

//         return resp;
//     }

// private:
// };

} // namespace IPC

#endif // _IPC_CLIENT_SOCKET_HPP_