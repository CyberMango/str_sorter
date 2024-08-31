#ifndef _REQUESTS_HANDLER_HPP_
#define _REQUESTS_HANDLER_HPP_

#include <cerrno>
#include <cstdint>
#include <iostream>
#include <string>

#include "../ipc/ipc.hpp"

class requests_handler {
public:
    requests_handler():
        m_server {}
    {
    }

    int32_t initialize() { return m_server.start_server(); }

    int32_t run();

private:
    IPC_server m_server;

    int32_t wait_for_request(
        uint32_t& uid, request_type& type, std::string& data);
    uint32_t send_data_to_client(uint32_t uid, std::string data);
    int32_t store_data(uint32_t uid, std::string data);
    int32_t fetch_data(uint32_t uid, uint32_t data_len, std::string& data);
};

#endif // _REQUESTS_HANDLER_HPP_