#pragma once

#include "client.hpp"

#include <atomic>
#include <poll.h>
#include <vector>

namespace clients {

    const int MAX_CLIENTS = 64;

    // Singleton
    struct clients_s {
        public:
            std::atomic<int>              number_of_clients;
            std::vector<pollfd>           p_clients;
            std::vector<client::client_t> c_clients;

            clients_s(const clients_s &) = delete;
            clients_s &operator=(const clients_s &) = delete;

            static clients_s &get_instance()
            {
                static clients_s s;
                return s;
            }

        private:
            clients_s();
    };

    void init();
    int  add(const int fd);
    void reset(const int index);

} // END namespace Clients