#ifndef _IPC_SOCKET_COMMON_HPP_
#define _IPC_SOCKET_COMMON_HPP_

/*** Includes ***/
#include <unistd.h>

#include "../utility/simple_logger.hpp"

namespace IPC {

/*** Constants ***/
#define ADDRESS_SEPARATOR (':')

/*** Definitions ***/

class socket_guard {
public:
    socket_guard(int socket_fd = 0):
        m_fd(socket_fd)
    {
    }
    ~socket_guard()
    {
        debug_print("closing socket %d\n", static_cast<int>(m_fd));
        close(m_fd);
    }

    int m_fd;
};

} // namespace IPC

#endif // _IPC_SOCKET_COMMON_HPP_