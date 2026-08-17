#pragma once

class ExprNode;
class NumberNode;
class StringNode;
class VariableNode;
class BinaryOpNode;
class UnaryOpNode;
class CallNode;
class ArrayAccessNode;

class StatementNode;
class BlockNode;
class AssignNode;
class CheckZeroNode;
class WhileNode;
class VariableDeclNode;
class RobotCommandNode;
class ReturnNode;
class FunctionDeclNode;
class ExprStatementNode;

class INodeVisitor
{
public:
    virtual void visit(const ExprNode& node) = 0;
    virtual void visit(const NumberNode& node) = 0;
    virtual void visit(const StringNode& node) = 0;
    virtual void visit(const VariableNode& node) = 0;
    virtual void visit(const BinaryOpNode& node) = 0;
    virtual void visit(const UnaryOpNode& node) = 0;
    virtual void visit(const CallNode& node) = 0;
    virtual void visit(const ArrayAccessNode& node) = 0;

    virtual void visit(const StatementNode& node) = 0;
    virtual void visit(const BlockNode& node) = 0;
    virtual void visit(const AssignNode& node) = 0;
    virtual void visit(const CheckZeroNode& node) = 0;
    virtual void visit(const WhileNode& node) = 0;
    virtual void visit(const VariableDeclNode& node) = 0; 
    virtual void visit(const RobotCommandNode& node) = 0;
    virtual void visit(const ReturnNode& node) = 0;
    virtual void visit(const FunctionDeclNode& node) = 0;
    virtual void visit(const ExprStatementNode& node) = 0;

    virtual ~INodeVisitor() = default;
};