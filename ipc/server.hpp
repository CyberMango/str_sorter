#ifndef _SERVER_HPP_
#define _SERVER_HPP_

/*** Includes ***/
#include <string>

#include "client.hpp"
#include "common.hpp"

namespace IPC {

/*** Constants ***/
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT (1234)
#define MAX_CONNECTIONS (10)

/*** Definitions ***/
class server {
public:
    virtual int32_t start_server() = 0;
    virtual int32_t wait_for_connection(IPC::client& connection) = 0;
};

} // namepsace IPC

#endif // _SERVER_HPP_