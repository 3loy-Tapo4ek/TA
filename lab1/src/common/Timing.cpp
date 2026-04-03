#include "Timing.hpp"

#include <chrono>
#include <iostream>
#include <string>

void print_take_statistics_milliseconds(const char* label, const double duration_ms)
{
    std::cerr << "Сбор статистики (\"" << label << "\"): " << duration_ms << " ms\n";
}

void print_take_statistics_milliseconds(
    const char* label,
    const std::chrono::steady_clock::time_point t0,
    const std::chrono::steady_clock::time_point t1)
{
    const double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
    std::cerr << "Сбор статистики (\"" << label << "\"): " << ms << " ms\n";
}

std::string GenerateCode(std::size_t target_length)
{
    const std::string base_expr = "int a := 5 + 5\n"; 
    
    std::string result;
    result.reserve(target_length);

    size_t full_blocks = target_length / base_expr.length();
    size_t remainder = target_length % base_expr.length();

    for (size_t i = 0; i < full_blocks; ++i)
    {
        result += base_expr;
    }

    if (remainder > 0)
    {
        result.append(remainder, ' ');
    }

    return result;
}
