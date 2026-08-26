#pragma once

#include "StatementNode.hpp"
#include <memory>
#include <vector>

class ReturnNode : public StatementNode
{
private:
    std::unique_ptr<ExprNode> expression_;
public:
    explicit ReturnNode(std::unique_ptr<ExprNode> expression)
    : expression_(std::move(expression)) {}

    const std::unique_ptr<ExprNode>& getExpression() const noexcept { return expression_; }

    //overriding
    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }

};