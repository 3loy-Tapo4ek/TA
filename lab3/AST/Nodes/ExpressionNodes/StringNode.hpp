#pragma once

#include "ExprNode.hpp"
#include <memory>
#include <string>

class StringNode : public ExprNode
{
private:
    std::string value_;
public:
    explicit StringNode(std::string value) : value_(std::move(value)) {}

    //visitor pattern
    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }

    const std::string& getValue() const noexcept { return value_;}
};