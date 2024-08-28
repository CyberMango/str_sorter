#include "requests_handler.hpp"
#include "../ipc/ipc.hpp"
#include <thread>

int32_t requests_handler::run()
{
    int32_t status = 0;
    uint32_t client_uid = 0;
    request_type type = IPC::INVALID;
    std::string request_data {};
    std::string response_data {};

    // TODO find a nice stop condition.
    while (true) {
        /* Client tells us who he is. In this version we are ok with that.
        There is not enough time to implement security mechanism and such.
        Even if we were to create an established connection with each client
        there would be security holes without a whole system that handles
        the security and not pretending problems.
        */
        status = wait_for_request(client_uid, type, request_data);
        if (0 != status) {
            return status;
        }

        // TODOO move this whole handling to a new thread each time.
        // TODO try to make it a switch-case again
        // Client wants to send data.
        if (request_type::SEND == type) {
            status = store_data(client_uid, request_data);
            if (0 != status) {
                return status;
            }
            // Client wants to receive data.
        } else if (request_type::RECEIVE == type) {
            errno = 0;
            char* end = nullptr;
            uint32_t data_len = static_cast<uint32_t>(
                std::strtoll(request_data.c_str(), &end, 10));
            /* TODOO dont actually stop the server due to this error on
            release. you need to print an error message and only stop on
            unrecoverable errors. here and everywhere you are the one
            generating the error.
            */
            if (0 != errno) {
                // TODO return a better error code maybe.
                return ERANGE;
            }
            status = fetch_data(client_uid, data_len, response_data);
            if (0 != status) {
                return status;
            }

            status = send_data_to_client(client_uid, response_data);
            if (0 != status) {
                return status;
            }
        } else {
            // TODOO
            return EINVAL;
        }
    }
}

// TODO
int32_t requests_handler::store_data(uint32_t uid, std::string data)
{
    return 0;
}

// TODO
int32_t requests_handler::fetch_data(
    uint32_t uid, uint32_t data_len, std::string& data)
{
    data = std::string(data_len, 'a');
    return 0;
}