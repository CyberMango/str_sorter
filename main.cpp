/*** Includes ***/
#include "client/simple_client.hpp"
#include "server/requests_handler.hpp"

#include <unistd.h>

/*** Functions ***/
int main(int argc, char* argv[])
{
    requests_handler server {};
    simple_client client {};

    server.initialize();

    auto server_handler = fork();
    if (0 == server_handler) {
        server.run();
    }

    std::string data_to_send { "initial data" };
    std::string received_data {};
    client.send(data_to_send);
    data_to_send = "second_data";
    client.send(data_to_send);

    client.recv(5, received_data);
    std::cout << "client got " << received_data << "\n";
    client.recv(50, received_data);
    std::cout << "client got " << received_data << "\n";
}