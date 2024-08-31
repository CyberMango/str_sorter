#ifndef _IPC_SERVER_HPP_
#define _IPC_SERVER_HPP_

#include <string>

class IPC_server {
public:
    enum class request_type : uint16_t {
        INVALID = 0,
        SEND,
        RECEIVE,
    };
    struct message {
    public:
        uint32_t id;
        uint32_t uid;
        IPC_server::request_type type;
        std::string data;
    };

    virtual IPC_server() = 0;

    virtual int32_t start_server() = 0;
    virtual int32_t wait_for_request() = 0;
    virtual int32_t send_request(IPC_server_socket::message const& request) = 0;
};

#endif // _IPC_HPP_