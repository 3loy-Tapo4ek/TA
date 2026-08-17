#pragma once

#include "Node.hpp"

enum class UnaryOp
{
    Dereference, //*
    AddressOf,   //& 
    SizeOf       //?
};

enum class BinaryOp
{
    Add,        // +
    Subtract,   // -
    Multiply,   // *
    Divide,     // /
    Remainder, // %
    Concat,     // + (strings)
    Equal,      // =
    LessThan,   // <
    GreaterThan // >
};

class ExprNode : public Node
{
private:
public:
    
};
