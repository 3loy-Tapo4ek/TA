#pragma once

#include "Node.hpp"
#include <optional>


//class for LITERAL
class ANode : public Node
{
private:
    std::optional<char> symbol_;
public:
    explicit ANode(std::optional<char> symbol = std::nullopt) : symbol_(symbol) {}

    //visitor pattern
    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }
    std::optional<char> getSymbol() const {return symbol_;}

    std::unique_ptr<Node> clone() const override { return std::make_unique<ANode>(symbol_); };

};