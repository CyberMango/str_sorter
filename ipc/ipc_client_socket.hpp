#ifndef _IPC_CLIENT_SOCKET_HPP_
#define _IPC_CLIENT_SOCKET_HPP_

#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

#include "../utility/simple_logger.hpp"
#include "ipc_server.hpp"

/*TODO this is a rushed client a real client needs:
- its own message struct
- its own metadata struct.
- interface separated from sockets.
*/
class IPC_client_socket {
public:
    IPC_server::response send_request(IPC_server::message const& request)
    {
        char data_buf[2048] = { 0 };
        IPC_server::message_metadata metadata_out = { 0 };
        IPC_server::message_metadata metadata_in = { 0 };
        IPC_server::response resp = { 0 };

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            std::cerr << "Could not create socket" << std::endl;
            return resp;
        }

        // Define server address
        sockaddr_in server_adr;
        server_adr.sin_addr.s_addr = inet_addr(SERVER_IP);
        server_adr.sin_family = AF_INET;
        server_adr.sin_port = htons(SERVER_PORT);

        // Connect to remote server
        if (connect(sock, (struct sockaddr*)&server_adr, sizeof(server_adr))
            < 0) {
            std::cerr << "Connection failed" << std::endl;
            return resp;
        }

        std::cout << "Connected to server" << std::endl;

        // Send some data
        metadata_out.data_length = request.data.length();
        metadata_out.type = request.type;
        metadata_out.uid = request.uid;
        memmove(data_buf, &metadata_out, sizeof(metadata_out));
        memmove(data_buf + sizeof(metadata_out),
            request.data.c_str(),
            request.data.length());
        data_buf[sizeof(metadata_out) + request.data.length()] = '\0';
        if (send(
                sock, data_buf, sizeof(metadata_out) + request.data.length(), 0)
            < 0) {
            std::cerr << "Send failed" << std::endl;
            return resp;
        }

        std::cout << "Message sent" << std::endl;

        // Receive a reply from the server
        char server_reply[2000];
        ssize_t reply_len
            = recv(sock, server_reply, sizeof(server_reply) - 1, 0);
        if (reply_len < 0) {
            std::cerr << "Receive failed" << std::endl;
            return resp;
        }

        // Null-terminate the received data
        server_reply[reply_len] = '\0';

        std::cout << "Server reply: " << server_reply << std::endl;

        memmove(&metadata_in, server_reply, sizeof(metadata_in));

        resp.id = 0;
        resp.type = metadata_in.type;
        resp.data = server_reply + sizeof(metadata_in);

        return resp;
    }

private:
};

#endif // _IPC_CLIENT_SOCKET_HPP_