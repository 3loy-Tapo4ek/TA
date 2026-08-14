#pragma once

#include "StatementNode.hpp"
#include <memory>
#include <vector>

class BlockNode : public StatementNode
{
private:
    std::vector<std::unique_ptr<StatementNode>> statements_;
public:
    explicit BlockNode(std::vector<std::unique_ptr<StatementNode>> statements)
    : statements_(std::move(statements)) {}

    const std::vector<std::unique_ptr<StatementNode>>& getStatements() const noexcept { return statements_; }

    //overriding
    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }

};