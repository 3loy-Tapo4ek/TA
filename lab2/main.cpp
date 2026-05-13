#include <iostream>
#include "Regex.hpp"
#include "GraphVisualizer.hpp"

int main() {
    try {
        regex_engine::Regex re;
        
        // 1. Компиляция (РВ -> НКА -> ДКА)
        std::cout << "Compiling pattern..." << std::endl;
        re.compile("a(<b>c...)|d");

        // 2. Поиск (findall)
        std::string text = "accc d acc";
        auto matches = re.findall(text, regex_engine::WithGroups{});

        std::cout << "Found " << matches.size() << " matches:" << std::endl;
        for (const auto& m : matches) {
            std::cout << "Full match: [" << m.full_match << "]" << std::endl;
            try {
                std::cout << "  Group 'b': " << m["b"] << std::endl;
            } catch (...) {}
        }

        // 3. Восстановление (K-пути)
        std::cout << "Restored Regex: " << re.toRegexString() << std::endl;

        regex_engine::GraphVisualizer::exportNFA(re.getNFA(), "nfa.dot");
        regex_engine::GraphVisualizer::exportDFA(re.getDFA(), "dfa.dot");

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}