#ifndef _IPC_SOCKET_SERVER_HPP_
#define _IPC_SOCKET_SERVER_HPP_

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>

#include "../../utility/simple_logger.hpp"
#include "../server.hpp"
#include "socket_common.hpp"

/*** Constants ***/
#define MAX_CONNECTIONS (10)

namespace IPC {

class socket_server : public IPC::server {
public:
    socket_server():
        m_socket { nullptr },
        m_socket_address { nullptr }
    {
    }
    virtual int32_t start_server(std::string address) override;
    virtual int32_t wait_for_connection(
        std::unique_ptr<IPC::client>& connection) override;

private:
    std::unique_ptr<IPC::socket_guard> m_socket;
    std::unique_ptr<struct sockaddr_in> m_socket_address;
};

} // namepsace IPC

#endif // _IPC_SOCKET_SERVER_HPP_