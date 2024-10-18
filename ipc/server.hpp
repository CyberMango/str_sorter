#ifndef _IPC_SERVER_HPP_
#define _IPC_SERVER_HPP_

/*** Includes ***/
#include <memory>
#include <string>

#include "client.hpp"
#include "common.hpp"

namespace IPC {

/*** Definitions ***/

/**
 * A server object for inter-process-communication.
 */
class server {
public:
    virtual ~server()
    {
    }
    /**
     * Set up the server to be able to listen for connections.
     *
     * address[in]: Address to attach to the server.
     */
    virtual int32_t start_server(std::string address) = 0;
    /**
     * Wait for a single connection to the server.
     *
     * connection[out]: Client object for communication with the new connection.
     */
    virtual int32_t wait_for_connection(
        std::unique_ptr<IPC::client>& connection)
        = 0;
};

} // namepsace IPC

#endif // _IPC_SERVER_HPP_