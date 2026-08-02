#pragma once

#include "Node.hpp"
#include <memory>

class OrNode : public Node
{
private:
    std::unique_ptr<Node> left_child_;
    std::unique_ptr<Node> right_child_;
public:
    OrNode(std::unique_ptr<Node> left_child, std::unique_ptr<Node> right_child)
    : left_child_(std::move(left_child)), right_child_(std::move(right_child)) {} ;

    std::string ToString() const override { return left_child_->ToString() + "|" + right_child_->ToString();};
    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }
    const std::unique_ptr<Node>& getLeftChild() const { return left_child_; }
    const std::unique_ptr<Node>& getRightChild() const { return right_child_; }
};