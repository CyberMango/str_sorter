#include "data_handler.hpp"

#include <algorithm>
#include <cerrno>
#include <mutex>
#include <shared_mutex>

#include "../utility/simple_logger.hpp"

data_handler::client_data::client_data(client_data&& other)
{
    debug_print("in move constructor of client_data\n");
    std::lock_guard<std::mutex> lock(other.m_data_lock);
    m_data = std::move(other.m_data);
}

data_handler::client_data& data_handler::client_data::operator=(
    client_data&& other)
{
    debug_print("in move operator= of client_data\n");
    std::lock_guard<std::mutex> lock(other.m_data_lock);
    m_data = std::move(other.m_data);

    return *this;
}

void data_handler::client_data::store_data(std::string const& data)
{
    std::lock_guard<std::mutex> lock(m_data_lock);

    m_data.append(data);
    std::sort(m_data.begin(), m_data.end());
}

std::string data_handler::client_data::fetch_data(std::size_t length)
{
    std::lock_guard<std::mutex> lock(m_data_lock);

    auto out = m_data.substr(0, length);
    // TODO check this works and does not throw error on short m_data.
    m_data = m_data.substr(std::min(length, m_data.size()));

    return out;
}

int32_t data_handler::store_data(uint32_t uid, std::string const& data)
{
    int32_t status = 0;

    status = this->verify_exists(uid);
    if (0 != status) {
        return status;
    }

    debug_print("storing \"%s\" in data of user %u\n", data.c_str(), uid);
    {
        std::shared_lock<std::shared_mutex> r_lock(m_clients_data_lock);
        m_clients_data[uid].store_data(data);
    }

    return 0;
}

int32_t data_handler::fetch_data(
    uint32_t uid, std::size_t data_len, std::string& data)
{
    int32_t status = 0;

    status = this->verify_exists(uid);
    if (0 != status) {
        return status;
    }

    debug_print("fetching %lu bytes for user %u\n", data_len, uid);
    {
        std::shared_lock<std::shared_mutex> r_lock(m_clients_data_lock);
        auto& client_data = m_clients_data[uid];
        data = client_data.fetch_data(data_len);
    }

    return 0;
}

bool data_handler::is_uid_stored(uint32_t uid)
{
    std::shared_lock<std::shared_mutex> r_lock(m_clients_data_lock);

    auto uid_iter = m_clients_data.find(uid);
    if (m_clients_data.end() == uid_iter) {
        return false;
    }
    return true;
}

int32_t data_handler::add_uid(uint32_t uid)
{
    std::unique_lock<std::shared_mutex> w_lock(m_clients_data_lock);

    bool success = m_clients_data.try_emplace(uid).second;
    if (!success) {
        error_print("emplacing value in m_clients_data failed\n");
        return EIO;
    }

    return 0;
}

int32_t data_handler::verify_exists(uint32_t uid)
{
    if (!is_uid_stored(uid)) {
        return add_uid(uid);
    }

    return 0;
}