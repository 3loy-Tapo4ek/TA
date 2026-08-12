#pragma once

#include "AutomataStruct.hpp"
#include <string>
#include <set>
#include <map>
#include <queue>

enum class OperationType 
{ 
    Difference, 
    Intersection 
};


class FAOperator
{
private:
    static std::set<char> getAlphabet(const FA& fa);

    static std::set<char> getExistingSymbols(const State& state);

    //creates complete FA from given FA
    static FA makeComplete(const FA& fa, const std::set<char>& alphabet);
    static void CreateDeadState(FA& fa, const std::set<char>& alphabet, size_t& dead_id);

    //BFS for 2 given FA
    static std::set<std::pair<size_t, size_t>> getReachable(const FA& fa_1, const FA& fa_2, const std::set<char>& alphabet);
    static size_t getNextState(const FA& fa, size_t state_id, char symbol);

    static FA doLogicalOperation(const FA& fa_1, const FA& fa_2);
    static bool isAcceptableState(OperationType operation, bool acc1, bool acc2);

    //helpers for K-path
    static std::string unitingTheArcs(size_t i, size_t j, const FA& fa);
    static std::string combinePath(std::string i_to_k, std::string k_to_k, std::string k_to_j);
    static std::string combineUnion(const std::string& a, const std::string& b);

    //just deleates epsilones or converts r|e = ?
    static std::string cleanupRegex(std::string expr);
    static void replaceAll(std::string& str, const std::string& from, const std::string& to);
    static std::string wrapIfNeeded(const std::string& str);




public:
    FAOperator() {};

    static FA MakeComplement(const FA& fa);
    static FA MakeDifference(const FA& fa_1, const FA& fa_2);
    static bool isIsomorphic(const FA& fa1, const FA& fa2);

    
    static std::string ReconstructRegex(const FA& fa);
};