#include "response.hpp"

#include "clients.hpp"

#include <mutex>
#include <string>
#include <unistd.h>

std::mutex response::write_mutex;

void response::echo(const int index)
{
    auto &clients = clients::clients_s::get_instance();
    response::send(index, clients.c_clients[index].header);
}

void response::send(const int index, const std::string &msg)
{
    size_t result = 0, total = 0;

    if (msg.size() == 0)
        return;

    auto &clients = clients::clients_s::get_instance();
    std::lock_guard<std::mutex> lk(response::write_mutex);
    while (total < msg.size()) {
        result = write(clients.p_clients[index].fd, msg.c_str(), msg.size() + 1);
        if (result > 0) {
            total += result;
        } else if (result == 0) {
            clients::reset(index);
            return;
        } else if (result < 0) {
            clients::reset(index);
            return;
        }
    }
}