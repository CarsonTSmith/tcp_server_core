#pragma once

#include <netinet/in.h>
#include <string>

namespace tcp_socket {

    const int PORT = 8085;

    const std::string SERVER_FULL_MSG      = "Server is full";
    const std::string CLIENT_CONNECTED_MSG = "Client connected";

    int  start(struct sockaddr_in *addr);
    void do_accept(const int sockfd, struct sockaddr_in *addr);
    void do_poll();

} // END namespace tcp_server