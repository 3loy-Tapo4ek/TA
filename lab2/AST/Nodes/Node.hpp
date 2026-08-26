#pragma once

#include <string>
#include <memory>
#include "INodeVisitor.hpp"

//Interface class
class Node
{
public:
    //for visitor pattern
    virtual void accept(INodeVisitor& visitor) const = 0;

    virtual std::unique_ptr<Node> clone() const = 0;

    virtual ~Node() = default;
};