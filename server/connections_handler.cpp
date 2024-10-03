#include "connections_handler.hpp"

#include <cstdint>
#include <memory>
#include <thread>

#include "../ipc/server.hpp"
#include "../utility/simple_logger.hpp"

int32_t connections_handler::wait_for_connection(
    std::unique_ptr<IPC::client>& connection)
{
    int32_t status = 0;

    if (!m_server_started) {
        status = m_server->start_server(m_address);
        if (0 != status) {
            return status;
        }
        m_server_started = true;
    }

    status = m_server->wait_for_connection(connection);
    if (0 != status) {
        return status;
    }

    return 0;
}