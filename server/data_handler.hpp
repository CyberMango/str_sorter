#ifndef _DATA_HANDLER_HPP_
#define _DATA_HANDLER_HPP_

#include <map>
#include <mutex>
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
        std::mutex mtx;
        std::string data;
    };
    std::map<uint32_t, data_handler::client_data> m_clients_data;
};

#endif // _DATA_HANDLER_HPP_