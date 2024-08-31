#include "data_handler.hpp"
#include <algorithm>

int32_t data_handler::store_data(uint32_t uid, std::string const& data)
{
    //TODO race condition for adding new UIDs.
    data_handler::client_data& client = m_clients_data[uid];
    std::lock_guard<std::mutex> lock(client.mtx);

    client.data = client.data + data;
    std::sort(client.data.begin(), client.data.end());

    return 0;
}