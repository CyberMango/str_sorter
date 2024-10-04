#include <ctime>
#include <iostream>
#include <memory>
#include <thread>

#include "../socket_client.hpp"
#include "../socket_server.hpp"
#include "client.hpp"
#include "server.hpp"

using namespace std;

void server_routine()
{
    int32_t status = 0;
    unique_ptr<IPC::server> server = make_unique<IPC::socket_server>();
    unique_ptr<IPC::client> client = nullptr;
    IPC::message in_msg {};
    IPC::message out_msg { .type = IPC::message_type::RESPONSE,
        .uid = 0,
        .data = vector<uint8_t>({ 75, 73, 78, 71 }) };

    status = server->start_server("127.0.0.1:1234");
    if (0 != status) {
        return;
    }

    server->wait_for_connection(client);

    client->recv_message(in_msg);
    client->send_message(out_msg);
    client->disconnect();

    cout << "server received:" << in_msg.data.cbegin().base() << endl;
}

void client_routine()
{
    sleep(2);

    IPC::message in_msg {};
    IPC::message out_msg { .type = IPC::message_type::RESPONSE,
        .uid = 0,
        .data = vector<uint8_t>({ 71, 85, 89, 69 }) };
    unique_ptr<IPC::client> client = make_unique<IPC::socket_client>();

    client->connect_to_server("127.0.0.1:1234");
    client->send_message(out_msg);
    client->recv_message(in_msg);
    client->disconnect();

    cout << "client received:" << in_msg.data.cbegin().base() << endl;
}

int main()
{
    thread server_thread(server_routine);
    thread client_thread(client_routine);

    server_thread.join();
    client_thread.join();
}