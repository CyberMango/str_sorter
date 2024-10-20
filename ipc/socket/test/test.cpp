#include <ctime>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <vector>

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

int16_t get_random_port()
{
    std::mt19937 rng {};
    std::uniform_int_distribution<uint16_t> uint_dist;

    auto x = time(nullptr);
    rng.seed(static_cast<uint32_t>(x));
    return uint_dist(rng) / (65536 / 1000) + 1000;
}

bool test_client_creation()
{
    IPC::socket_client client1 {};

    return true;
}

bool test_client_failed_connection()
{
    int32_t status = 0;
    IPC::socket_client client {};

    status = client.connect_to_server("127.0.0.1:1122");
    if (0 == status) {
        return false;
    }

    return true;
}

bool test_server_start()
{
    int32_t status = 0;
    IPC::socket_server server {};
    std::string address { "127.0.0.1:" };

    address.append(std::to_string(get_random_port()));

    status = server.start_server(address);
    if (0 != status) {
        return false;
    }

    return true;
}

typedef bool (*test_func)();

static std::vector<std::pair<test_func, std::string>> test_suite {
    { test_client_creation, "test_client_creation" },
    { test_client_failed_connection, "test_client_failed_connection" },
    { test_server_start, "test_server_start" }
};

int main()
{
    cout << "Starting to run test suite\n";

    for (auto& test : test_suite) {
        try {
            bool result = test.first();
            if (!result) {
                cout << "test named " << test.second << " failed.\n";
            }
        } catch (...) {
            cout << "test named " << test.second
                 << " failed with an exception.\n";
        }
    }
}