#include <iostream>
#include <memory>
#include <thread>

#include "client.hpp"
#include "server.hpp"
#include "socket_client.hpp"
#include "socket_server.hpp"

using namespace std;

void server_routine()
{
    unique_ptr<IPC::server> server = make_unique<IPC::socket_server>();
    unique_ptr<IPC::client> client = make_unique<IPC::socket_client>();
    IPC::message in_msg {};
    IPC::message out_msg { .type = IPC::message_type::RESPONSE,
        .uid = 0,
        .data = vector<uint8_t>({ 60, 61, 62, 63 }) };

    server->start_server();

    server->wait_for_connection(*client);

    client->recv_message(in_msg);
    client->send_message(out_msg);
    client->disconnect();

    cout << "server received:" << in_msg.data.cbegin().base() << endl;
}

void client_routine()
{
    IPC::message in_msg {};
    IPC::message out_msg { .type = IPC::message_type::RESPONSE,
        .uid = 0,
        .data = vector<uint8_t>({ 70, 71, 72, 73 }) };
    unique_ptr<IPC::client> client = make_unique<IPC::socket_client>();

    client->connect_to_server("localhost:1234");
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