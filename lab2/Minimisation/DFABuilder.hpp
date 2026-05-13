#pragma once

#include "NFA.hpp"
#include "DFA.hpp"
#include <set>
#include <map>
#include <memory>

namespace regex_engine {

class DFABuilder {
public:
    static std::unique_ptr<DFA> build(const NFA& nfa);
    static std::unique_ptr<DFA> minimize(const DFA& dfa);
    static std::unique_ptr<DFA> buildComplement(const DFA& dfa);
    static std::unique_ptr<DFA> buildDifference(const DFA& dfa1, const DFA& dfa2);

private:
    static std::set<int> epsilonClosure(const std::set<int>& states, const NFA& nfa);
    static std::set<int> move(const std::set<int>& states, char symbol, const NFA& nfa);
    static std::set<char> getAlphabet(const NFA& nfa);
    static std::set<char> getDFAAlphabet(const DFA& dfa);
};

} // namespace regex_engine