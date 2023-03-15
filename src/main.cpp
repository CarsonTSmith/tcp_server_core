#include "clients.hpp"
#include "tcp_socket.hpp"

#include <netinet/in.h>
#include <signal.h>
#include <thread>

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in addr;

    signal(SIGPIPE, SIG_IGN);
    clients::init();
    sockfd = tcp_socket::start(&addr);
    std::thread accept_thread(tcp_socket::do_accept, sockfd, &addr);
    tcp_socket::do_poll();

    return 0;
}