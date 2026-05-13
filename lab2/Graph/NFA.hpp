#pragma once

#include <vector>
#include <memory>
#include <string>

namespace regex_engine {

constexpr char EPSILON = '\0';

struct State;

struct Transition {
    char symbol;
    State* to;
};

struct State {
    int id;
    std::vector<Transition> transitions;
    std::string capture_start;
    std::string capture_end;

    explicit State(int id);
    void addTransition(char symbol, State* to);
    void addEpsilonTransition(State* to);
};

struct NFAFragment {
    State* start;
    State* accept;
};

class NFA {
public:
    NFA();
    State* createState();

    const std::vector<std::unique_ptr<State>>& getStates() const;
    
    State* startState;
    State* acceptState;

private:
    std::vector<std::unique_ptr<State>> states_;
};

} // namespace regex_engine