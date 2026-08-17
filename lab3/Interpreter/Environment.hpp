#pragma once

#include "Value.hpp"
#include <unordered_map>
#include <string>
#include <memory>
#include <stdexcept>
#include "RuntimeExceptions.hpp"

class FunctionDeclNode;

struct Variable
{
    Value value;
    bool is_mutable = true;
};

class Environment;

struct Function
{
    const FunctionDeclNode& declaration_;
    std::shared_ptr<Environment> closure_;
};

class Environment
{
private:
    std::unordered_map<std::string, Variable> bindings_;
    std::unordered_map<std::string, Function> functions_;
    std::shared_ptr<Environment> parent_ = nullptr;

public:
    Environment() = default;
    explicit Environment(std::shared_ptr<Environment> parent) : parent_(std::move(parent)) {}

    void Define(const std::string& name, Value val, bool isMutable);
    void Set(const std::string& name, Value val);

    void defineFunction(const std::string& name, const FunctionDeclNode& decl, std::shared_ptr<Environment> closure);

    const Function& resolveFunction(const std::string& name) const;

    Value get(const std::string& name) const;

    //help funcs
    Variable& resolve(const std::string& name);
    const Variable& resolve(const std::string& name) const;
};


