#ifndef HELPER_H
#define HELPER_H

#include <chrono>
#include <iostream>
#include <string>

static uint64_t printEpochInMS(const std::string& a_msg, uint64_t a_index, bool a_print = true)
{
    auto time = std::chrono::system_clock::now();
    auto since_epoch = time.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch).count();

    if (a_print)
    {
        std::cout << a_msg << " - " << a_index << " , time in ms: " << milliseconds << std::endl;
    }

    return milliseconds;
}

#endif // HELPER_H
