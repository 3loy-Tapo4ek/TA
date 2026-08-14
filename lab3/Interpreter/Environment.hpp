#pragma once

#include "Value.hpp"
#include <unordered_map>
#include <string>
#include <memory>
#include <stdexcept>

struct Variable
{
    Value value;
    bool is_mutable = true;
};

class Environment
{
private:
    std::unordered_map<std::string, Variable> bindings_;
    std::shared_ptr<Environment> parent_ = nullptr;

    //help funcs
    Variable& resolve(const std::string& name);
    const Variable& resolve(const std::string& name) const;
public:
    Environment() = default;
    explicit Environment(std::shared_ptr<Environment> parent) : parent_(std::move(parent)) {}

    void Define(const std::string& name, Value val, bool isMutable);
    void Set(const std::string& name, Value val);

    Value get(const std::string& name) const;
};
