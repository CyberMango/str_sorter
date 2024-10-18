#ifndef _DATA_HANDLER_HPP_
#define _DATA_HANDLER_HPP_

#include <cstdint>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <utility>
#include <vector>

#include "../../utility/simple_logger.hpp"

class data_handler {
public:
    data_handler() = default;

    /**
     * Store new data for a specified UID.
     */
    int32_t store_data(uint32_t uid, std::string const& data);
    /**
     * Fetch sorted data of a specified UID.
     */
    int32_t fetch_data(uint32_t uid, std::size_t data_len, std::string& data);

private:
    class client_data {
    public:
        client_data() = default;
        client_data(client_data&& other);
        client_data& operator=(client_data&& other);

        void store_data(std::string const& data);
        std::string fetch_data(std::size_t length);

    private:
        std::mutex m_data_lock;
        std::string m_data;
    };

    bool is_uid_stored(uint32_t uid);
    int32_t add_uid(uint32_t uid);
    int32_t verify_exists(uint32_t uid);

    std::shared_mutex m_clients_data_lock;
    std::map<uint32_t, class data_handler::client_data> m_clients_data;
};

#endif // _DATA_HANDLER_HPP_