#ifndef _REQUESTS_HANDLER_HPP_
#define _REQUESTS_HANDLER_HPP_

#include <cerrno>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

#include "../ipc/server.hpp"

class connections_handler {
public:
    connections_handler(
        std::unique_ptr<IPC::server> server, std::string address):
        m_server { std::move(server) },
        m_server_started(false),
        m_address { address }
    {
    }

    int32_t wait_for_connection(std::unique_ptr<IPC::client>& connection);

private:
    std::unique_ptr<IPC::server> m_server;
    bool m_server_started;
    std::string m_address;
};

#endif // _REQUESTS_HANDLER_HPP_