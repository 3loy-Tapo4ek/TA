#pragma once

#include "IRecognizer.hpp"
#include <chrono>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <memory>

#include "RegexRecognizer.hpp"
#include "FlexRecognizer.hpp"
#include "SMCRecognizer.hpp"

class Benchmark
{
public:
    static void run(const std::vector<size_t>& string_lengths, size_t iterations = 50);

private:
    static std::string generateTestString(size_t target_length);
    static double measureTimeNs(IRecognizer& recognizer, const std::string& input, size_t iterations);
};