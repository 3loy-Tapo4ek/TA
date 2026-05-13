#include "DFA.hpp"

namespace regex_engine {

DFAState::DFAState(int id, bool is_accepting, std::set<int> nfa_states)
    : id(id), is_accepting(is_accepting), nfa_states(std::move(nfa_states)) {}

void DFAState::addTransition(char symbol, DFAState* to) {
    transitions[symbol] = to;
}

DFA::DFA() : startState(nullptr) {}

DFAState* DFA::createState(bool is_accepting, const std::set<int>& nfa_states) {
    int id = static_cast<int>(states_.size());
    states_.push_back(std::make_unique<DFAState>(id, is_accepting, nfa_states));
    return states_.back().get();
}

std::vector<DFAState*> DFA::getAcceptStates() const {
    std::vector<DFAState*> accept_states;
    for (const auto& state : states_) {
        if (state->is_accepting) {
            accept_states.push_back(state.get());
        }
    }
    return accept_states;
}

const std::vector<std::unique_ptr<DFAState>>& DFA::getStates() const {
    return states_;
}

} // namespace regex_engine