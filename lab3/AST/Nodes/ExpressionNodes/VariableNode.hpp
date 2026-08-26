#pragma once

#include "ExprNode.hpp"
#include <memory>
#include <string>

class VariableNode : public ExprNode
{
private:
    std::string name_;
public:
    explicit VariableNode(std::string name) : name_(std::move(name)) {}

    //visitor pattern
    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }

    const std::string& getName() const noexcept {return name_;}
};