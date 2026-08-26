#pragma once

#include "AutomataStruct.hpp"
#include <unordered_set>
#include <queue>
#include <map>
#include <set>
#include <limits>


class DFABuilder
{
private:
    std::map<std::set<size_t>, size_t> dfa_state_ids_; 
    std::queue<std::set<size_t>> unmarked_dfa_states_;

    //get closure for one state and for set of state (overload)
    std::set<size_t> getEpsilonClosure(const FA& nfa, size_t start_id);
    std::set<size_t> getEpsilonClosure(const FA& nfa, const std::set<size_t>& start_states);

    //collectinf alphabet
    std::set<char> getAlphabet(const FA& nfa);
    
    //symbol closure
    std::set<size_t> move(const FA& nfa, const std::set<size_t>& states, char symbol);

    //help methods for minDFA
    std::vector<size_t> buildStateToGroup(size_t num_states, const std::vector<std::set<size_t>>& partitions);
    std::vector<size_t> getResultOfDelta(const FA& dfa, size_t state_id, const std::set<char>& alphabet, const std::vector<size_t>& state_to_group);
    FA reconstructMinDFA(const FA& dfa, const std::vector<std::set<size_t>>& partitions, const std::vector<size_t>& state_to_group);

public:
    DFABuilder() {};
    ~DFABuilder() = default;

    FA buildDFA(const FA& nfa);
    FA buildMinDFA(const FA& dfa);
};