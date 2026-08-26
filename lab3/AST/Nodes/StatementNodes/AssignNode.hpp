#pragma once

#include "StatementNode.hpp"
#include <memory>
#include <vector>

class AssignNode : public StatementNode
{
private:
    std::unique_ptr<ExprNode> target_;
    std::unique_ptr<ExprNode> value_;
public:
    AssignNode(std::unique_ptr<ExprNode> target, std::unique_ptr<ExprNode> value)
    : target_(std::move(target)), value_(std::move(value)) {}

    const std::unique_ptr<ExprNode>& getTarget() const noexcept { return target_; }
    const std::unique_ptr<ExprNode>& getValue() const noexcept { return value_; }

    //overriding
    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }

};