#include "data_handler.hpp"

#include <algorithm>
#include <mutex>
#include <shared_mutex>

// int32_t data_handler::store_data(uint32_t uid, std::string const& data)
// {
//     // TODO race condition for adding new UIDs.
//     data_handler::client_data& client = m_clients_data[uid];
//     std::lock_guard<std::mutex> lock(client.data_lock);

//     client.data = client.data + data;
//     std::sort(client.data.begin(), client.data.end());

//     return 0;
// }

bool data_handler::is_uid_stored(uint32_t uid)
{
    std::shared_lock<std::shared_mutex> r_lock(m_clients_data_lock);

    auto uid_iter = m_clients_data.find(uid);
    if (m_clients_data.end() == uid_iter) {
        return false;
    }
    return true;
}