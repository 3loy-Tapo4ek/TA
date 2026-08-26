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

    //visitor pattern
    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }
    const std::unique_ptr<Node>& getLeftChild() const { return left_child_; }
    const std::unique_ptr<Node>& getRightChild() const { return right_child_; }

    //overriding
    std::unique_ptr<Node> clone() const override { return std::make_unique<OrNode>(left_child_->clone(), right_child_->clone()); };

};