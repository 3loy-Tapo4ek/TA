#pragma once

#include "DFA.hpp"
#include "NFA.hpp"
#include "RegexMatch.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>

namespace regex_engine {

// Флаг для перегрузки функции findall
struct WithGroups {};

class Regex {
public:
    // Компиляция регулярного выражения из строки
    void compile(const std::string& pattern);
    
    // Восстановление регулярного выражения методом K-путей
    std::string toRegexString() const;

    // Перегрузка 1: Без доступа к группам (возвращает просто строки)
    std::vector<std::string> findall(const std::string& text) const;

    // Перегрузка 2: С доступом к группам (возвращает объекты RegexMatch)
    std::vector<RegexMatch> findall(const std::string& text, WithGroups) const;

private:
    std::unique_ptr<NFA> nfa_;
    std::unique_ptr<DFA> dfa_;

    // Вспомогательный метод поиска пути в НКА для извлечения групп
    bool extractGroupsWithNFA(State* state, const std::string& text, int pos,
                              std::map<std::string, int>& start_positions,
                              RegexMatch& match, std::set<State*>& visited_epsilons) const;
};

} // namespace regex_engine