#pragma once

#include "NFA.hpp"
#include "DFA.hpp"
#include <string>

namespace regex_engine {

class GraphVisualizer {
public:
    // Экспорт НКА в файл .dot
    static void exportNFA(const NFA& nfa, const std::string& filename);

    // Экспорт ДКА в файл .dot
    static void exportDFA(const DFA& dfa, const std::string& filename);

private:
    // Вспомогательный метод для экранирования спецсимволов для DOT
    static std::string escapeSymbol(char symbol);
};

} // namespace regex_engine