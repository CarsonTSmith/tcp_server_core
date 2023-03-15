#include "tcp_socket.hpp"

#include "clients.hpp"
#include "request.hpp"

#include <errno.h>
#include <fcntl.h>
#include <functional>
#include <future>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

static void process(clients::clients_s &clients, int num_fds) {
    for (int i = 0; (num_fds > 0) && (i < clients::MAX_CLIENTS); ++i) {
        if (clients.p_clients[i].revents & POLLIN) {
            // TODO: delegate handle_request to threadpool in the future
            // for now use std::async
            auto result = std::async(std::launch::async, request::handle_request, i);
            num_fds--;
        }
    }
}

int tcp_socket::start(struct sockaddr_in *addr)
{
    int sockfd, opt = 1;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            fprintf(stderr, "socket failed");
            exit(errno);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            fprintf(stderr, "setsockopt");
            exit(errno);
    }

    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)addr, sizeof(*addr)) < 0) {
            fprintf(stderr, "bind failed");
            exit(errno);
    }

    if (listen(sockfd, clients::MAX_CLIENTS) < 0) {
            fprintf(stderr, "listen failed");
            exit(errno);
    }

    printf("server successfully started\n");
    return sockfd;
}

void tcp_socket::do_accept(const int sockfd, struct sockaddr_in *addr)
{
    socklen_t addrsz = sizeof(*addr);;
    int clientfd, flags;

    while (1) {
        if ((clientfd = accept(sockfd, (struct sockaddr *)addr, &addrsz)) < 0) {
            fprintf(stderr, "tcp_server::do_accept() failed");
            exit(-1);
        }

        flags = fcntl(clientfd, F_GETFL, 0);
        fcntl(clientfd, F_SETFL, flags | O_NONBLOCK);

        if (clients::add(clientfd) < 0)
            close(clientfd);
    }
}

void tcp_socket::do_poll()
{
    int num_fds;
    auto &clients = clients::clients_s::get_instance();

    while (1) {
		num_fds = poll(clients.p_clients.data(),
                       clients.number_of_clients,
                       1); // timeout so when new clients connect they are polled
		if (num_fds > 0) {
			process(clients, num_fds);
		} else if (num_fds < 0) { // poll error
			fprintf(stderr, "rd_from_clients() poll error\n%d", errno);
			exit(errno);
		} else if (num_fds == 0) { // no data sent yet, poll again
			continue;
		}
	}
}