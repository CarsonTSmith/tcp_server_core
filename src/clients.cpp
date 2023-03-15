#include "clients.hpp"

#include "client.hpp"

#include <poll.h>
#include <unistd.h>
#include <vector>

clients::clients_s::clients_s()
{
    p_clients.resize(MAX_CLIENTS);
    c_clients.resize(MAX_CLIENTS);

    for (auto &p_client: p_clients)
        p_client.fd = -1;
}

void clients::init()
{
    clients::clients_s::get_instance();
}

int clients::add(const int fd)
{
    auto &clients = clients_s::get_instance();
    if (clients.number_of_clients > (int)clients.p_clients.size())
        return -1;

    for (auto &p_client: clients.p_clients) {
        if (p_client.fd == -1) {
            p_client.fd = fd;
            p_client.events = POLLIN | POLLPRI;
            clients.number_of_clients++;
            break;
        }
    }

    return 0;
}

void clients::reset(const int index) {
    auto &clients = clients_s::get_instance();
    clients.p_clients[index].fd      = -1;
    clients.p_clients[index].events  = 0;
    clients.p_clients[index].revents = 0;

    memset(clients.c_clients[index].body, 0, sizeof(clients.c_clients[index].body));
    memset(clients.c_clients[index].header, 0, sizeof(clients.c_clients[index].header));
    clients.c_clients[index].header_done     = false;
    clients.c_clients[index].body_length     = 0;
    clients.c_clients[index].header_bytes_rd = 0;
    clients.c_clients[index].body_bytes_rd   = 0;
    
    clients.number_of_clients--;

    close(clients.p_clients[index].fd);
}