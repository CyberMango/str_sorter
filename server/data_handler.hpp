#ifndef _DATA_HANDLER_HPP_
#define _DATA_HANDLER_HPP_

#include <cstdint>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <utility>
#include <vector>

class data_handler {
public:
    data_handler() = default;

    int32_t store_data(uint32_t uid, std::string const& data);
    int32_t fetch_data(uint32_t uid, uint32_t data_len, std::string& data);

private:
    class client_data {
    public:
        void store_data(std::string data);
        std::string fetch_data(std::size_t length);

    private:
        std::mutex data_lock;
        std::string data;
    };

    bool is_uid_stored(uint32_t uid);
    void add_uid(uint32_t);

    std::shared_mutex m_clients_data_lock;
    std::map<uint32_t, data_handler::client_data> m_clients_data;
};

#endif // _DATA_HANDLER_HPP_