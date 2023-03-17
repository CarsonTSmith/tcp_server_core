#pragma once

#include <mutex>
#include <string>

namespace response {

    extern std::mutex write_mutex;

    void send(const int index, const std::string &msg);
    void echo(const int index);

} // END Namespace response