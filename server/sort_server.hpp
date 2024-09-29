#ifndef _SORT_SERVER_HPP_
#define _SORT_SERVER_HPP_

#include <cstdint>
#include <memory>

#include "../ipc/server.hpp"
#include "../ipc/socket_server.hpp"
#include "../utility/simple_logger.hpp"
#include "data_handler.hpp"
#include "requests_handler.hpp"

class sort_server {
public:
    sort_server() = default;

    int32_t run()
    {
        int32_t status = 0;
        std::unique_ptr<IPC_server_socket> server
            = std::make_unique<IPC_server_socket>();

        status = m_requests.initialize(std::move(server));
        if (0 != status) {
            error_print("Requests_handler initialization failed\n");
            return status;
        }
        return m_requests.run();
    }

private:
    requests_handler m_requests;
};

#endif // _SORT_SERVER_HPP_