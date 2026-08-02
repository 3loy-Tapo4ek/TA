#pragma once

#include <string>
#include "INodeVisitor.hpp"

//Interface class
class Node
{
public:
    virtual std::string ToString() const = 0;

    //for visitor pattern
    virtual void accept(INodeVisitor& visitor) const = 0;

    virtual ~Node() = default;
};