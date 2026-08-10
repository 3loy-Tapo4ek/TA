#include "CoolRegex.hpp"
#include <iostream>

int main()
{
    CoolRegex re("(a|(b...m){14})?c{20}...");
    re.compile();

    std::string code = "abcabcabcabdcabcabcabcabcabcabcabcabcabcabcabmephidipkacabcabcabcabcabcabcabc";

    re.visualizePipeline("complex_test");

    auto matches = re.findAll(code);

    std::cout << "=== НАЙДЕННЫЕ ВЫЗОВЫ ФУНКЦИЙ ===" << std::endl;
    std::cout << "Всего совпадений: " << matches.size() << std::endl;
    for (size_t i = 0; i < matches.size(); ++i) {
        std::cout << i + 1 << ". " << matches[i] << std::endl;
    }

    return 0;
}