#pragma once

#include "Node.hpp"
#include <memory>

class StarNode : public Node
{
private:
    std::unique_ptr<Node> child_;
public:
    explicit StarNode(std::unique_ptr<Node> child) : child_(std::move(child)) {};

    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }
    
    const std::unique_ptr<Node>& getChild() const { return child_; }

    //overriding
    std::unique_ptr<Node> clone() const override { return std::make_unique<StarNode>(child_->clone()); };

};
