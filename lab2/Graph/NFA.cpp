#include "NFA.hpp"

namespace regex_engine {

// State implementation
State::State(int id) : id(id) {}

void State::addTransition(char symbol, State* to) {
    transitions.push_back({symbol, to});
}

void State::addEpsilonTransition(State* to) {
    transitions.push_back({EPSILON, to});
}

// NFA implementation
NFA::NFA() : startState(nullptr), acceptState(nullptr) {}

State* NFA::createState() {
    int id = static_cast<int>(states_.size());
    states_.push_back(std::make_unique<State>(id));
    return states_.back().get();
}

const std::vector<std::unique_ptr<State>>& NFA::getStates() const {
    return states_;
}

} // namespace regex_engine