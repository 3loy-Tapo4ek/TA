#pragma once

#include "ExprNode.hpp"

class NumberNode : public ExprNode
{
private:
    int value_;
public:
    explicit NumberNode(int value) : value_(value) {}

    //for visitor pattern
    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }

    int getValue() const noexcept { return value_; }
};