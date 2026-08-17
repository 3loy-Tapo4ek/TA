#pragma once

#include "StatementNode.hpp"
#include "ExprNode.hpp"
#include <memory>

class ExprStatementNode : public StatementNode
{
private:
    std::unique_ptr<ExprNode> expression_;
public:
    explicit ExprStatementNode(std::unique_ptr<ExprNode> expr) : expression_(std::move(expr)) {}

    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }

    const std::unique_ptr<ExprNode>& getExpression() const noexcept { return expression_; }
};
