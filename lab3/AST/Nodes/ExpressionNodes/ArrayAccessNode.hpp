#pragma once

#include "ExprNode.hpp"
#include <string>
#include <memory>

class ArrayAccessNode : public ExprNode
{
private:
    std::string array_name_;
    std::unique_ptr<ExprNode> index_;
public:
    explicit ArrayAccessNode(std::string array_name, std::unique_ptr<ExprNode> index)
    : array_name_(std::move(array_name)), index_(std::move(index)) {}

    //for visitor pattern
    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }

    const std::string& getArrayName() const noexcept { return array_name_; }
    const std::unique_ptr<ExprNode>& getIndex() const noexcept { return index_; }
};