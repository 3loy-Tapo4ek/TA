#pragma once

#include <stdexcept>
#include <string>
#include "Value.hpp"

struct ReturnSignal
{
    Value value;
    explicit ReturnSignal(Value val) : value(std::move(val)) {}
};

struct BreakSignal {};

class RuntimeError : public std::runtime_error
{
public:
    explicit RuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

