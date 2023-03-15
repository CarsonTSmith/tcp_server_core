#pragma once

#include <stdint.h>
#include <string>
#include <string.h>

namespace client {

    constexpr int BODY_SIZE   = 8192;
    constexpr int HEADER_SIZE = 8;

    struct client_t {
        char         header[HEADER_SIZE];
        char         body[BODY_SIZE];
        bool         header_done = false;
        uint32_t body_length     = 0;
        uint32_t header_bytes_rd = 0;
        uint32_t body_bytes_rd   = 0;
        
        client_t()
        {
            memset(header, 0, sizeof(header));
            memset(body, 0, sizeof(body));
        }

        void reset()
        {
            memset(header, 0, sizeof(header));
            memset(body, 0, sizeof(body));
            header_done     = false;
            body_length     = 0;
            header_bytes_rd = 0;
            body_bytes_rd   = 0;
        }
    };

} // END namespace client
