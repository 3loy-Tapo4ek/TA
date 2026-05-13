#include "Regex.hpp"
#include "FlexLexerAdapter.hpp"
#include "Parser.hpp"
#include "NFABuilder.hpp"
#include "DFABuilder.hpp"
#include "RegexRestorer.hpp"
#include <sstream>

namespace regex_engine {

void Regex::compile(const std::string& pattern) {
    // 1. Лексический анализ
    std::istringstream stream(pattern);
    FlexLexerAdapter lexer(stream);
    
    // 2. Синтаксический анализ (AST)
    Parser parser(lexer);
    auto ast = parser.parse();
    
    // 3. Построение НКА
    nfa_ = std::make_unique<NFA>();
    NFABuilder nfa_builder(*nfa_);
    ast->accept(nfa_builder);
    
    auto fragment = nfa_builder.getResult();
    nfa_->startState = fragment.start;
    nfa_->acceptState = fragment.accept;
    
    // 4. Построение и минимизация ДКА
    auto raw_dfa = DFABuilder::build(*nfa_);
    dfa_ = DFABuilder::minimize(*raw_dfa);
}

std::string Regex::toRegexString() const {
    if (!dfa_) throw std::runtime_error("Regex not compiled.");
    return RegexRestorer::restore(*dfa_);
}

std::vector<std::string> Regex::findall(const std::string& text) const {
    std::vector<std::string> results;
    if (!dfa_ || !dfa_->startState) return results;

    int i = 0;
    int n = text.length();
    
    while (i < n) {
        int last_accept_pos = -1;
        DFAState* curr = dfa_->startState;
        
        // Проверка на совпадение пустой строки в текущей позиции
        if (curr->is_accepting) last_accept_pos = i - 1;

        int j = i;
        while (j < n) {
            char c = text[j];
            auto it = curr->transitions.find(c);
            if (it != curr->transitions.end()) {
                curr = it->second;
                if (curr->is_accepting) {
                    last_accept_pos = j;
                }
                j++;
            } else {
                break;
            }
        }

        if (last_accept_pos != -1 && last_accept_pos >= i) {
            // Нашли непустое совпадение
            int match_length = last_accept_pos - i + 1;
            results.push_back(text.substr(i, match_length));
            i = last_accept_pos + 1; // Сдвигаемся за конец совпадения (непересекающиеся)
        } else if (last_accept_pos == i - 1) {
            // Нашли пустое совпадение
            results.push_back("");
            i++; // Чтобы не зациклиться
        } else {
            // Совпадений нет, идем дальше
            i++;
        }
    }
    return results;
}

std::vector<RegexMatch> Regex::findall(const std::string& text, WithGroups) const {
    std::vector<RegexMatch> results;
    // Сначала находим все полные совпадения через быструю перегрузку
    std::vector<std::string> raw_matches = findall(text);

    // Для каждого совпадения запускаем НКА для извлечения групп
    for (const std::string& match_str : raw_matches) {
        RegexMatch match_obj(match_str);
        
        if (nfa_ && nfa_->startState) {
            std::map<std::string, int> start_positions;
            std::set<State*> visited;
            extractGroupsWithNFA(nfa_->startState, match_str, 0, start_positions, match_obj, visited);
        }
        
        results.push_back(match_obj);
    }
    
    return results;
}

// Рекурсивный поиск в глубину (DFS) по НКА для извлечения захваченных групп
bool Regex::extractGroupsWithNFA(State* state, const std::string& text, int pos,
                                 std::map<std::string, int>& start_positions,
                                 RegexMatch& match, std::set<State*>& visited_epsilons) const {
    
    // Обработка маркеров групп
    if (!state->capture_start.empty()) {
        start_positions[state->capture_start] = pos;
    }
    if (!state->capture_end.empty()) {
        std::string name = state->capture_end;
        if (start_positions.count(name)) {
            int start_pos = start_positions[name];
            match.addGroup(name, text.substr(start_pos, pos - start_pos));
        }
    }

    // Если достигли конца текста и принимающего состояния НКА
    if (pos == text.length() && state == nfa_->acceptState) {
        return true; 
    }

    // Проходим по всем переходам
    for (const auto& transition : state->transitions) {
        if (transition.symbol == EPSILON) {
            // Защита от бесконечных эпсилон-циклов
            if (visited_epsilons.find(transition.to) == visited_epsilons.end()) {
                visited_epsilons.insert(transition.to);
                if (extractGroupsWithNFA(transition.to, text, pos, start_positions, match, visited_epsilons)) {
                    return true;
                }
                visited_epsilons.erase(transition.to);
            }
        } else if (pos < text.length() && transition.symbol == text[pos]) {
            // Переход по символу
            std::set<State*> empty_visited; // При сдвиге позиции эпсилон-история очищается
            if (extractGroupsWithNFA(transition.to, text, pos + 1, start_positions, match, empty_visited)) {
                return true;
            }
        }
    }

    return false;
}

} // namespace regex_engine