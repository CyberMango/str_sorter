#include "socket_common.hpp"

#include <arpa/inet.h>
#include <cstdint>
// #include <netinet/in.h>

int32_t IPC::analyze_address(std::string address, std::string& ip, int& port)
{
    char* endptr = nullptr;
    std::size_t separator_index = address.find(ADDRESS_STR_SEPARATOR);
    if (std::string::npos == separator_index) {
        error_print("couldnt find %c in address - format is wrong\n",
            ADDRESS_STR_SEPARATOR);
        return EINVAL;
    }

    ip = address.substr(0, separator_index);
    port = strtol(address.substr(separator_index + 1).c_str(), &endptr, 10);
    if ('\0' != *endptr) {
        error_print(
            "failed extracting port from address %s\n", address.c_str());
        return EINVAL;
    }

    return 0;
}

int32_t IPC::get_socket_address(
    std::string address, sockaddr_in* socket_address)
{
    int32_t status = 0;
    int unix_status = 0;
    std::string ip_address {};
    int port = 0;

    status = IPC::analyze_address(address, ip_address, port);
    if (0 != status) {
        return status;
    }
    socket_address->sin_family = AF_INET;
    socket_address->sin_port = htons(port);

    // Convert the ASCII address to binary address.
    errno = 0;
    unix_status
        = inet_pton(AF_INET, ip_address.c_str(), &(socket_address->sin_addr));
    status = static_cast<int32_t>(errno);
    if (0 == unix_status) {
        error_print("invalid network address: %s\n", ip_address.c_str());
        return EINVAL;
    }
    if (unix_status < 0) {
        error_print("inet_pton failed. errno=%d\n", status);
        return status;
    }

    return 0;
}