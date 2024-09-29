#ifndef _IPC_CLIENT_HPP_
#define _IPC_CLIENT_HPP_

/*** Includes ***/
#include <cstdint>
#include <string>

#include "common.hpp"

namespace IPC {

/*** Classes ***/

class client {
public:
    virtual int32_t connect_to_server(std::string address) = 0;
    virtual int32_t disconnect() = 0;
    virtual int32_t send_message(IPC::message const& message) = 0;
    virtual int32_t recv_message(IPC::message& message) = 0;
};

} // namepsace IPC

#endif // _IPC_CLIENT_HPP_