/*** Includes ***/
#include <iostream>
#include <unistd.h>

#include "client/simple_client.hpp"
#include "ipc/socket_server.hpp"
// #include "server/requests_handler.hpp"
#include "server/sort_server.hpp"
#include "utility/simple_logger.hpp"
using namespace std;

/*** Functions ***/
// IPC test

void test_server()
{
    // IPC_server_socket server {};
    // IPC::server::message msg = { 0 };
    // IPC::server::response resp = { 0 };
    // server.start_server();
    // char* end = nullptr;

    // while (true) {
    //     auto req = server.wait_for_request(msg);
    //     cout << "data received from client: " << msg.data << endl;
    //     if (msg.type == IPC::server::request_type::RECEIVE) {
    //         auto len = static_cast<uint32_t>(
    //             std::strtoll(msg.data.c_str(), &end, 10));
    //         resp.data = string(len, 'a');
    //         resp.id = msg.id;
    //         resp.type = IPC::server::request_type::RECEIVE;
    //         server.send_response(resp);
    //     }
    // }

    sort_server server {};
    server.run();
}

void test_client()
{
    string data {};
    str_client client {};
    cout << "client sending1" << endl;
    client.send(string_view { "aaaaabbbbb" });
    cout << "client sending2" << endl;
    client.send("hhhhhhhhhh");

    cout << "client receiving" << endl;
    client.recv(5, data);
    cout << "received: " << data << endl;
    client.recv(56, data);
    cout << "received: " << data << endl;
}

int main(int argc, char* argv[])
{

    auto fd = fork();
    if (0 == fd) {
        test_client();
    } else {
        test_server();
    }

    return 0;
}

// int main(int argc, char* argv[])
// {
//     requests_handler server {};
//     str_client client {};

//     server.initialize();

//     auto server_handler = fork();
//     if (0 == server_handler) {
//         server.run();
//     }

//     std::string data_to_send { "initial data" };
//     std::string received_data {};
//     client.send(data_to_send);
//     data_to_send = "second_data";
//     client.send(data_to_send);

//     client.recv(5, received_data);
//     std::cout << "client got " << received_data << "\n";
//     client.recv(50, received_data);
//     std::cout << "client got " << received_data << "\n";
// }