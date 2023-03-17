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

// reset tcp connection and reset client
void clients::reset(const int index) {
    auto &clients = clients_s::get_instance();
    close(clients.p_clients[index].fd);
    clients.p_clients[index].fd      = -1;
    clients.p_clients[index].events  = 0;
    clients.p_clients[index].revents = 0;

    clients.c_clients[index].reset();
    
    clients.number_of_clients--;
}