#pragma once

#include <chrono>
#include <cstddef>
#include <string>

void print_take_statistics_milliseconds(
    const char* label,
    std::chrono::steady_clock::time_point t0,
    std::chrono::steady_clock::time_point t1);

std::string GenerateCode(std::size_t target_length);
