#ifndef _IPC_SERVER_HPP_
#define _IPC_SERVER_HPP_

#include <string>

/*** Constants ***/
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT (1242)
#define MAX_CONNECTIONS (10)

/*** Classes ***/
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

    struct __attribute__((packed)) message_metadata {
        uint32_t uid;
        IPC_server::request_type type;
        uint32_t data_length;
    };

    struct response {
    public:
        uint32_t id;
        IPC_server::request_type type;
        std::string data;
    };

    virtual int32_t start_server() = 0;
    virtual int32_t wait_for_request(IPC_server::message& request) = 0;
    virtual int32_t send_response(IPC_server::response const& to_send) = 0;
};

#endif // _IPC_HPP_