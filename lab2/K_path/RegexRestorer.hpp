#pragma once

#include "DFA.hpp"
#include <string>
#include <vector>

namespace regex_engine {

// Вспомогательный класс для умной склейки регулярных выражений
class RegexExpr {
public:
    std::string str;
    bool is_empty_set; // Обозначает пустое множество (Ø), не путать с пустой строкой
    bool is_epsilon;   // Обозначает пустую строку (ε)
    int precedence;    // Приоритет оператора (для правильной расстановки скобок)

    RegexExpr(); // По умолчанию пустое множество
    static RegexExpr makeEmptySet();
    static RegexExpr makeEpsilon();
    static RegexExpr makeChar(char c);

    // Операции с автоматическим упрощением
    static RegexExpr unite(const RegexExpr& a, const RegexExpr& b);
    static RegexExpr concat(const RegexExpr& a, const RegexExpr& b);
    static RegexExpr kleene(const RegexExpr& a); // Оператор ...
};

class RegexRestorer {
public:
    // Главный метод восстановления РВ из ДКА
    static std::string restore(const DFA& dfa);
};

} // namespace regex_engine