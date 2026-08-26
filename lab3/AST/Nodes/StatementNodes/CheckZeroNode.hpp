#pragma once

#include "StatementNode.hpp"
#include <memory>

class CheckZeroNode : public StatementNode
{
private:
    std::unique_ptr<ExprNode> condition_;
    std::unique_ptr<StatementNode> body_;
    std::unique_ptr<StatementNode> insteadBody_;

public:
    CheckZeroNode(std::unique_ptr<ExprNode> cond, std::unique_ptr<StatementNode> body, std::unique_ptr<StatementNode> insteadBody = nullptr)
    : condition_(std::move(cond)), body_(std::move(body)), insteadBody_(std::move(insteadBody)) {}

    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }

    const std::unique_ptr<ExprNode>& getCondition() const noexcept { return condition_; }
    const std::unique_ptr<StatementNode>& getBody() const noexcept { return body_; }
    const std::unique_ptr<StatementNode>& getInsteadBody() const noexcept { return insteadBody_; }
};