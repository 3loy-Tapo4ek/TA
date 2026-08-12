#pragma once

class ANode;
class ConcatNode;
class OrNode;
class StarNode;

class INodeVisitor
{
public:
    virtual void visit(const ANode& node) = 0;
    virtual void visit(const ConcatNode& node) = 0;
    virtual void visit(const OrNode& node) = 0;
    virtual void visit(const StarNode& node) = 0;

    virtual ~INodeVisitor() = default;
};