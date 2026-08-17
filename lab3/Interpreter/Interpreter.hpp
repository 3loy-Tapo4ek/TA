#pragma once

#include "INodeVisitor.hpp"
#include "Environment.hpp"
#include "Ast.hpp"
#include <memory>
#include "IRobotController.hpp"


class Interpreter : public INodeVisitor
{
private:
    std::shared_ptr<Environment> current_environment_;
    std::shared_ptr<Environment> global_environment_;
    IRobotController& robot_;
    Value last_evaluated_value_;

    Value makeBoolValue(bool condition);


    //overriding visitor functions
    void visit(const ExprNode& node) override;
    void visit(const NumberNode& node) override;
    void visit(const StringNode& node) override;
    void visit(const VariableNode& node) override;
    void visit(const BinaryOpNode& node) override;
    void visit(const UnaryOpNode& node) override;
    void visit(const CallNode& node) override;
    void visit(const ArrayAccessNode& node) override;

    void visit(const StatementNode& node) override;
    void visit(const BlockNode& node) override;
    void visit(const AssignNode& node) override;
    void visit(const CheckZeroNode& node) override;
    void visit(const WhileNode& node) override;
    void visit(const VariableDeclNode& node) override; 
    void visit(const RobotCommandNode& node) override;
    void visit(const ReturnNode& node) override;
    void visit(const FunctionDeclNode& node) override;
    void visit(const ExprStatementNode& node) override;

public:
    Interpreter(IRobotController& robot)
    :global_environment_(std::make_shared<Environment>()), current_environment_(global_environment_),
    robot_(robot) {};
    void Interpret(const StatementNode& root);

    Value Evaluate(const ExprNode& expression);

    void Execute(const StatementNode& statement);

};