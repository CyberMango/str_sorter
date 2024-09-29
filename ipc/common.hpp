#ifndef _IPC_COMMON_HPP_
#define _IPC_COMMON_HPP_

/*** Includes ***/
#include <cstdint>
#include <string>
#include <vector>

/*** Definitions ***/
namespace IPC {

enum class message_type : uint16_t {
    INVALID = 0,
    SEND,
    RECEIVE,
    RESPONSE,
};

struct __attribute__((packed)) message_metadata {
public:
    message_type type;
    uint32_t uid;
    uint32_t data_len;
};

struct message {
public:
    message_type type;
    uint32_t uid;
    std::vector<uint8_t> data;
};

} // namepsace IPC

#endif // _IPC_COMMON_HPP_