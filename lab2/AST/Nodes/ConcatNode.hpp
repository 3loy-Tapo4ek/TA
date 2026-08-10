#pragma once

#include "Node.hpp"
#include <memory>

class ConcatNode : public Node
{
private:
    std::unique_ptr<Node> left_child_;
    std::unique_ptr<Node> right_child_;
public:
    ConcatNode(std::unique_ptr<Node> left_child, std::unique_ptr<Node> right_child)
    : left_child_(std::move(left_child)), right_child_(std::move(right_child)) {} ;

    //for visitor pattern
    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }
    const std::unique_ptr<Node>& getLeftChild() const { return left_child_; }
    const std::unique_ptr<Node>& getRightChild() const { return right_child_; }

    //overriding
    std::unique_ptr<Node> clone() const override { return std::make_unique<ConcatNode>(left_child_->clone(), right_child_->clone()); };

};