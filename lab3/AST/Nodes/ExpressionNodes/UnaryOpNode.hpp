#pragma once

#include "ExprNode.hpp"
#include <memory>


class UnaryOpNode : public ExprNode
{
private:
    UnaryOp operation_;
    std::unique_ptr<ExprNode> operand_;
public:
    UnaryOpNode(UnaryOp operation, std::unique_ptr<ExprNode> operand)
    : operation_(operation), operand_(std::move(operand)) {}

    //for visitor pattern
    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }

    UnaryOp getOperation() const noexcept { return operation_; }
    const std::unique_ptr<ExprNode>& getOperand() const noexcept { return operand_; }
};