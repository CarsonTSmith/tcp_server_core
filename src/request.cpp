#include "request.hpp"

#include "clients.hpp"
#include "response.hpp"

#include <arpa/inet.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

constexpr int HEADER_DONE       = 0;
constexpr int HEADER_NOT_DONE   = -1;
constexpr int HEADER_READ_ERROR = -2;

constexpr int BODY_DONE         = 0;
constexpr int BODY_NOT_DONE     = -1;
constexpr int BODY_READ_ERROR   = -2;

static inline uint32_t convert_header_to_num(const char *header)
{
    char *endptr;
	int ret;

	ret = strtol(header, &endptr, 10);
	if (ret >= 0)
		return ret;

	return 0;
}

static inline int read_header(const int index)
{
    auto &clients = clients::clients_s::get_instance();
    const int bytesrd = read(clients.p_clients[index].fd,
                             clients.c_clients[index].header,
                             client::HEADER_SIZE - clients.c_clients[index].header_bytes_rd);
    if (bytesrd < 0) {
        if (errno == EAGAIN)
            return HEADER_NOT_DONE;

        return HEADER_READ_ERROR;
    }

    if (bytesrd == 0)
        return HEADER_READ_ERROR;

    clients.c_clients[index].header_bytes_rd += bytesrd;
    if (clients.c_clients[index].header_bytes_rd == client::HEADER_SIZE) {
        clients.c_clients[index].header_done = true;
        clients.c_clients[index].body_length = convert_header_to_num(clients.c_clients[index].header);
        return HEADER_DONE;
    }

    return HEADER_NOT_DONE;
}

static inline int read_body(const int index)
{
    auto &clients = clients::clients_s::get_instance();
    const auto bytesrd = read(clients.p_clients[index].fd,
                              clients.c_clients[index].body,
                              clients.c_clients[index].body_length - clients.c_clients[index].body_bytes_rd);
    if (bytesrd < 0) {
        if (errno == EAGAIN)
            return BODY_NOT_DONE; 

        return BODY_READ_ERROR;
    }

    if (bytesrd == 0)
        return BODY_READ_ERROR;

    clients.c_clients[index].body_bytes_rd += bytesrd;
    if (clients.c_clients[index].body_bytes_rd < clients.c_clients[index].body_length)
        return BODY_NOT_DONE;
    
    return BODY_DONE;
}

static inline void do_read_body(const int index)
{
    auto &clients = clients::clients_s::get_instance();
    int status = read_body(index);
    switch (status) {
    case BODY_DONE:
        // perform the request
        // then reset client buffers
        response::echo(index); // send the message body back to sender
        clients.c_clients[index].reset();
        break;
    case BODY_NOT_DONE:
        break; // go back to polling
    case BODY_READ_ERROR:
    default:
        clients::reset(index);
        break;
    }
}

static inline void do_read_header(const int index)
{
    int status = read_header(index);
    switch (status) {
    case HEADER_DONE:
        do_read_body(index);
        break;
    case HEADER_NOT_DONE:
        break; // go back to polling
    case HEADER_READ_ERROR:
    default:
        clients::reset(index);
        break;
    }
}

int request::handle_request(const int index)
{
    auto &clients = clients::clients_s::get_instance();
    if (clients.c_clients[index].header_done == true) {
        do_read_body(index);
    } else {
        do_read_header(index);
    }

    return 0;
}