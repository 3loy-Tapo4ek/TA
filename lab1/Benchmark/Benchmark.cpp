#include "Benchmark.hpp"

std::string Benchmark::generateTestString(size_t target_length)
{
    //correct expr
    const std::string prefix = "int a := 1 + ";
    const std::string suffix = "2";
    
    if (target_length <= prefix.length() + suffix.length())
    {
        return prefix + suffix;
    }

    size_t zeros_needed = target_length - prefix.length() - suffix.length();

    //int a := 1 + 000000...0002
    return prefix + std::string(zeros_needed, '0') + suffix;
}

double Benchmark::measureTimeNs(IRecognizer& recognizer, const std::string& input, size_t iterations)
{

    auto start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < iterations; i++)
    {
        recognizer.reset();
        auto result = recognizer.parseLine(input);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::nano> elapsed = end - start;

    return elapsed.count() / static_cast<double>(iterations);
}

#include <fstream>

void Benchmark::run(const std::vector<size_t>& string_lengths, size_t iterations)
{
    RegexRecognizer regex_rec;
    FlexRecognizer flex_rec;
    SMCRecognizer smc_rec;

    std::ofstream csv_file("benchmark_results.csv");
    csv_file << "Length,Regex,Flex,SMC\n";

    std::cout << "\n=== BENCHMARK (Iterations: " << iterations << ") ===\n";
    std::cout << std::left << std::setw(15) << "String Length" 
              << std::setw(20) << "Regex (ns)" 
              << std::setw(20) << "Flex (ns)" 
              << std::setw(20) << "SMC (ns)" << "\n";
    std::cout << std::string(75, '-') << "\n";

    for (size_t len : string_lengths)
    {
        std::string test_str = generateTestString(len);

        double t_regex = measureTimeNs(regex_rec, test_str, iterations);
        double t_flex  = measureTimeNs(flex_rec, test_str, iterations);
        double t_smc   = measureTimeNs(smc_rec, test_str, iterations);

        //print to console
        std::cout << std::left << std::setw(15) << len 
                  << std::setw(20) << std::fixed << std::setprecision(2) << t_regex 
                  << std::setw(20) << t_flex 
                  << std::setw(20) << t_smc << "\n";

        //write to csv
        csv_file << len << "," << t_regex << "," << t_flex << "," << t_smc << "\n";
    }
    std::cout << std::string(75, '-') << "\n";
    std::cout << "Results saved to benchmark_results.csv\n";
}