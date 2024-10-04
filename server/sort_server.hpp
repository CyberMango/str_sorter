#ifndef _SORT_SERVER_HPP_
#define _SORT_SERVER_HPP_

#include <cstdint>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "../ipc/server.hpp"
#include "../utility/simple_logger.hpp"
#include "connections_handler.hpp"
#include "data_handler.hpp"

class sort_server {
public:
    sort_server(std::string address);
    ~sort_server();

    int32_t run();

private:
    void client_routine(std::unique_ptr<IPC::client> connection);
    int32_t handle_message(
        IPC::message& in_msg, std::unique_ptr<IPC::client>& connection);
    int32_t handle_receive_message(
        IPC::message& in_msg, std::unique_ptr<IPC::client>& connection);

    std::unique_ptr<connections_handler> m_connections;
    std::unique_ptr<data_handler> m_data;
    std::vector<std::thread> m_routines;
};

#endif // _SORT_SERVER_HPP_