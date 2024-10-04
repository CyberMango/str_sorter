/** System tests for the project.
 *
 */

/*** Includes ***/
#include <cstring>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

#include "client/str_client.hpp"
#include "server/sort_server.hpp"
#include "utility/simple_logger.hpp"

using namespace std;

/*** Definitions ***/
typedef bool (*test_func)();

/*** Functions ***/

int16_t get_random_port()
{
    std::mt19937 rng {};
    std::uniform_int_distribution<uint16_t> uint_dist;

    auto x = time(nullptr);
    rng.seed(static_cast<uint32_t>(x));
    return uint_dist(rng) / (65536 / 1000) + 1000;
}

bool test_single_client()
{
    string address = string("127.0.0.1:") + std::to_string(get_random_port());

    sort_server server { address };
    str_client client {};

    std::thread server_thread { [&server]() { server.run(); } };

    auto client_routine = [address](str_client& client) {
        int32_t status = 0;
        std::string data {};

        sleep(2);
        status = client.connect(address);
        if (0 != status) {
            return false;
        }

        status = client.send("edcba");
        if (0 != status) {
            return false;
        }
        client.send("abcde");
        client.send("aaaaa");
        client.send("bbbbb");
        client.send("ccccc");

        client.recv(5, data);
        if (strncmp("aaaaa", data.c_str(), 6)) {
            return false;
        }

        client.recv(20, data);
        if (strncmp("aabbbbbbbcccccccddee", data.c_str(), 21)) {
            return false;
        }

        client.recv(500, data);
        if (strncmp("", data.c_str(), 1)) {
            return false;
        }

        return true;
    };

    bool result = client_routine(std::ref(client));

    // Dirty way to end the server. one day you will fix.
    auto th = server_thread.native_handle();
    pthread_cancel(th);
    server_thread.join();

    return result;
}

bool test_multiple_clients_one_user()
{
    string address = string("127.0.0.1:") + std::to_string(get_random_port());

    sort_server server { address };
    str_client client1 {};
    str_client client2 {};
    str_client client3 {};

    std::thread server_thread { [&server]() { server.run(); } };

    auto client_routine = [address](str_client& client) {
        int32_t status = 0;
        std::string data {};

        sleep(2);
        status = client.connect(address);
        if (0 != status) {
            return false;
        }

        status = client.send("edcba");
        if (0 != status) {
            error_print("send_failed\n");
            return false;
        }
        status = client.send("abcde");
        if (0 != status) {
            error_print("send_failed\n");
            return false;
        }
        status = client.send("aaaaa");
        if (0 != status) {
            error_print("send_failed\n");
            return false;
        }
        status = client.send("bbbbb");
        if (0 != status) {
            error_print("send_failed\n");
            return false;
        }
        status = client.send("ccccc");
        if (0 != status) {
            error_print("send_failed\n");
            return false;
        }

        status = client.recv(25, data);
        debug_print("@@@@ data got \"%s\"\n", data.c_str());
        if (0 != status) {
            error_print("receive_failed\n");
            return false;
        }

        return true;
    };

    std::thread client1_thread { client_routine, std::ref(client1) };
    std::thread client2_thread { client_routine, std::ref(client2) };
    std::thread client3_thread { client_routine, std::ref(client3) };

    // TODO for now check manually that nothing failed. too lazy to use future.

    client1_thread.join();
    client2_thread.join();
    client3_thread.join();

    // Dirty way to end the server. one day you will fix.
    auto th = server_thread.native_handle();
    pthread_cancel(th);
    server_thread.join();

    return true;
}

static std::vector<std::pair<test_func, std::string>> test_suite {
    { test_single_client, "test_single_client" },
    { test_multiple_clients_one_user, "test_multiple_clients_one_user" }
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