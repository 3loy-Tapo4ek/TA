#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <set>

namespace regex_engine {

struct DFAState {
    int id;
    bool is_accepting;
    
    // Переходы: символ -> состояние
    std::unordered_map<char, DFAState*> transitions;

    // Храним ID исходных состояний НКА для дебага и возможной логики захвата
    std::set<int> nfa_states;

    explicit DFAState(int id, bool is_accepting, std::set<int> nfa_states);
    void addTransition(char symbol, DFAState* to);
};

class DFA {
public:
    DFA();
    
    DFAState* createState(bool is_accepting, const std::set<int>& nfa_states);
    
    DFAState* startState;
    std::vector<DFAState*> getAcceptStates() const;
    const std::vector<std::unique_ptr<DFAState>>& getStates() const;

private:
    std::vector<std::unique_ptr<DFAState>> states_;
};

} // namespace regex_engine