#pragma once

#include <vector>
#include <memory>
#include <optional>

struct Transition
{
    size_t targer_state_id;
    std::optional<char> symbol;

};

struct State
{
    size_t id_;
    bool is_acceptable_;
    std::vector<Transition> transitions;
};

struct FAFragment
{
    size_t start_ptr_;
    size_t accept_ptr_;
};

struct FA
{
    std::vector<State> states_;
    size_t start_ptr_;
    size_t accept_ptr_; //Нужно учитывать, что при алгоритме МЯТ получается ровно одно принимающее и одно стартовое
};