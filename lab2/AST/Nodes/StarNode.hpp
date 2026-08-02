#pragma once

#include "Node.hpp"
#include <memory>

class StarNode : public Node
{
private:
    std::unique_ptr<Node> child_;
public:
    explicit StarNode(std::unique_ptr<Node> child) : child_(std::move(child)) {};

    std::string ToString() const override { return child_->ToString() + "..."; };

    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }
    
    const std::unique_ptr<Node>& getChild() const { return child_; }
};
