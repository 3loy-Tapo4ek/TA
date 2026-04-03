#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "lexer/Lexer.hpp"
#include "common/Recognizer.hpp"
#include "common/string_utils.hpp"
#include "common/Timing.hpp"

namespace
{
const char* kCodeFile = "../lab1/src/code.txt";
const char* kStatsFile = "stats.txt";
const int kIterations = 10;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Использование: " << argv[0]
                  << " <smc|flex|manual> [file|stdin]\n"
                  << "  file  — читать исходник из " << kCodeFile << " (по умолчанию)\n"
                  << "  stdin — одна строка с клавиатуры\n";
        return 1;
    }

    const std::string mode = to_lower(argv[1]);
    std::string source_kind = "file";
    if (argc >= 3)
        source_kind = to_lower(argv[2]);

    std::string code;

    if (source_kind == "file")
    {
        std::ifstream file(kCodeFile);
        if (!file.is_open())
        {
            std::cerr << "Не удалось открыть файл: " << kCodeFile << '\n';
            return 1;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        code = buffer.str();
    }
    else if (source_kind == "stdin")
    {
        std::getline(std::cin, code);
    }
    else if (source_kind == "gen")
    {
        size_t chars_count = 10000;

        if (argc >= 4)
        {
            chars_count = std::stoull(argv[3]);
        }

        code = GenerateCode(chars_count);
        std::cout << "Сгенерировано символов: " << code.length() << '\n';
    }
    else
    {
        std::cerr << "Неизвестный источник «" << argv[2]
                  << "». Ожидается: file или stdin.\n";
        return 1;
    }

    if (mode != "smc" && mode != "flex" && mode != "manual")
    {
        std::cerr << "Неизвестный режим «" << argv[1] << "». Ожидается: smc, flex или manual.\n";
        return 1;
    }
    std::vector<std::pair<bool, std::string>> final_stats;
    double total_time_ms = 0.0;

    for (int i = 0; i < kIterations; ++i)
    {
        std::unique_ptr<IRecognizer> recognizer;

        const auto t0 = std::chrono::steady_clock::now();
        
        if (mode == "smc")
        {
            std::unique_ptr<ILexer> lexer = std::make_unique<LexerRegex>(code);
            std::vector<Token> tokens = lexer->Run();
            recognizer = std::make_unique<RecognizerSMC>(std::move(tokens));
        }
        else if (mode == "flex")
        {
            recognizer = std::make_unique<RecognizerFlex>(code);
        }
        else if (mode == "manual")
        {
            recognizer = std::make_unique<Recognizer>(code);
        }

        const std::vector<std::pair<bool, std::string>> stats = recognizer->TakeStatistics();
        const auto t1 = std::chrono::steady_clock::now();

        std::chrono::duration<double, std::milli> ms = t1 - t0;
        total_time_ms += ms.count();

        if (i == 0)
            final_stats = stats;
    }

    double average_time_ms = total_time_ms / kIterations;

    std::cout << "Сбор статистики (\"" << mode << "\"): " 
              << average_time_ms << " ms"
              << " ms (среднее за " << kIterations << " прогонов)\n";

    std::ofstream stats_out(kStatsFile);
    if (!stats_out.is_open())
    {
        std::cerr << "Не удалось открыть для записи: " << kStatsFile << '\n';
        return 1;
    }

    for (const auto& it : final_stats)
        stats_out << it.first << " <----> " << it.second << '\n';

    std::cerr << "Статистика записана в " << kStatsFile << '\n';

    return 0;
}