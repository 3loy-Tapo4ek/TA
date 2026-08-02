#pragma once

#include "Node.hpp"

//class for LITERAL
class ANode : public Node
{
private:
    char symbol_;
public:
    explicit ANode(char symbol) : symbol_(symbol) {};

    std::string ToString() const override { return std::string(1, symbol_); }

    //visitor pattern
    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }
    char getSymbol() const { return symbol_;}
};