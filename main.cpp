/*** Includes ***/
#include <ctime>
#include <iostream>
#include <thread>
#include <unistd.h>

#include "client/str_client.hpp"
#include "ipc/socket/socket_client.hpp"
#include "server/sort_server.hpp"
#include "utility/simple_logger.hpp"
using namespace std;

/*** Constants ***/
#define ADDRESS "127.0.0.1:1238"

#define UNUSED(_var) (void)(_var)

/*** Functions ***/

void test_server()
{
    sort_server server { ADDRESS };
    server.run();
}

void test_client()
{
    string data {};
    auto ipc_client = std::make_unique<IPC::socket_client>();
    str_client client { std::move(ipc_client) };

    sleep(1);

    client.connect(ADDRESS);

    cout << "client sending1" << endl;
    client.send(string_view { "message1" });

    cout << "client sending2" << endl;
    client.send("second msg");

    cout << "client sending3" << endl;
    client.send("third");

    cout << "client sending4" << endl;
    client.send("4th time");

    cout << "client receiving" << endl;
    client.recv(5, data);
    cout << "received1: \"" << data << "\"" << endl;

    client.recv(56, data);
    cout << "received2: \"" << data << "\"" << endl;

    client.recv(4, data);
    cout << "received3: \"" << data << "\"" << endl;
}

int main(int argc, char* argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    std::thread server_thread(test_server);
    std::thread client_thread(test_client);

    server_thread.join();
    client_thread.join();

    return 0;
}