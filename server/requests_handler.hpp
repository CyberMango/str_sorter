#ifndef _REQUESTS_HANDLER_HPP_
#define _REQUESTS_HANDLER_HPP_

#include <cerrno>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

#include "../ipc/ipc_server.hpp"

class requests_handler {
public:
    requests_handler() = default;

    int32_t initialize(std::unique_ptr<IPC_server> server)
    {
        m_server = std::move(server);
        return m_server->start_server();
    }

    int32_t run();

private:
    std::unique_ptr<IPC_server> m_server;

    int32_t store_data(uint32_t uid, std::string data);
    int32_t fetch_data(uint32_t uid, uint32_t data_len, std::string& data);

    int32_t handle_request(IPC_server::message in_msg);
};

#endif // _REQUESTS_HANDLER_HPP_