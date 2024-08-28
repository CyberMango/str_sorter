#ifndef _IPC_HPP_
#define _IPC_HPP_

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <map>
#include <mutex>
#include <pair>
#include <string>
#include <vector>

// TODO make this an abstract class for interface, and implement in a socket_IPC
// TODO break this into a client class and server class.
class IPC {
public:
    enum class request_type : uint16_t {
        INVALID = 0,
        SEND,
        RECEIVE,
    };

    struct message {
        uint32_t uid;
        // Per client id for the interaction.
        uint16_t id;
        uint16_t type;
        std::string data;
    };

    IPC() = default;
    ~IPC()
    {
        // TODO close all sockets.
    }
    int32_t start_server();
    int32_t wait_for_request();

    // TODO temp fix until i break this into client and server classes.
    int32_t server_send_request(IPC::message const& request);
    int32_t client_send_request(IPC::message const& request);

private:
    struct __attribute__((packed)) message_metadata {
        uint32_t uid;
        uint16_t type;
        uint32_t data_length;
    };

    int m_server_fd;
    // Maps UIDs to a vector of active connections with their IDs.
    // TODO actually put the id in the key not the value. too late though.
    std::map<uint32_t, std::vector<std::pair<uint16_t, int>>> connections;
    std::mutex connections_mtx;

    int32_t IPC::get_address(struct sockaddr_in& address);
    uint16_t add_connection(uint32_t uid, int socket)
    {
        std::lock_guard<std::mutex> lock(connections_mtx);
        if (0 == connections.count(uid)) {
            // TODO make sure the value is not deleted after this line.
            connections[uid] = std::vector<std::pair<uint16_t, int>> {};
        }
        auto& con_vec = connections[uid];
        std::sort(con_vec.begin(), con_vec.end());
        uint16_t new_id = con_vec[con_vec.size() - 1].second + 1;
        con_vec.push_back(std::pair { new_id, socket });

        return new_id;
    }

    int pop_connection(uint32_t uid, uint16_t id)
    {
        std::lock_guard<std::mutex> lock(connections_mtx);
        int socket = 0;
        auto& con_vec = connections[uid];
        auto it = std::find_if(con_vec.begin(), con_vec.end(),
            [id](const std::pair<int, std::string>& elem) {
                return elem.first == id;
            });

        socket = it->second;
        con_vec.erase(it);

        return socket;
    }
};

#endif // _IPC_HPP_