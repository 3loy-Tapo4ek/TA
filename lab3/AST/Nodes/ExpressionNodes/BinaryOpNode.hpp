#pragma once

#include "ExprNode.hpp"
#include <memory>


class BinaryOpNode : public ExprNode
{
private:
    BinaryOp operation_;
    std::unique_ptr<ExprNode> left_;
    std::unique_ptr<ExprNode> right_;
public:
    BinaryOpNode(BinaryOp operation, std::unique_ptr<ExprNode> left, std::unique_ptr<ExprNode> right)
    : operation_(operation), left_(std::move(left)), right_(std::move(right)) {}

    //for visitor pattern
    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }

    BinaryOp getOperation() const noexcept { return operation_; }
    const std::unique_ptr<ExprNode>& getLeft() const noexcept { return left_; }
    const std::unique_ptr<ExprNode>& getRight() const noexcept { return right_; }
};