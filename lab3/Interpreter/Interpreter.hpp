#pragma once

#include "INodeVisitor.hpp"
#include "Environment.hpp"
#include "Ast.hpp"
#include <memory>
#include "IRobotController.hpp"
#include "ScopeGuard.hpp"


class Interpreter : public INodeVisitor
{
private:
    std::shared_ptr<Environment> current_environment_;
    std::shared_ptr<Environment> global_environment_;
    IRobotController& robot_;
    Value last_evaluated_value_;


    //helpers 

    //helpers for handling binary operations
    Value evaluateBinary(BinaryOp op, const Value& left, const Value& right);
    Value evaluateArifmetic(BinaryOp op, const Value& left, const Value& right);
    Value evaluatePointerArifmetic(BinaryOp op, const Value& left, const Value& right);
    Value evaluateComparison(BinaryOp op, const Value& left, const Value& right);

    //helpers for handling unary operations
    Value evaluateUnary(UnaryOp op, const ExprNode* operand_node, const Value& val);
    Value evaluateAddressOf(const ExprNode* operand_node);
    Value evaluateDereference(const Value& val);
    Value evaluateSizeOf(const Value& val);

    //helpers for handling defining
    Value getDefaultValue(DataType type);
    
    //helpers for assigment
    void executeAssignment(const ExprNode& target, const Value& value);
    void assignToVariable(const VariableNode& node, const Value& value);
    void assignToArray(const ArrayAccessNode& node, const Value& value);
    void assignToDereference(const UnaryOpNode& node, const Value& value);

    //helpers for call
    void executeFunctionCall(const CallNode& node, const Function& fn);
    void executeIteratorCall(const CallNode& node, const Function& fn);
    std::vector<Value> evaluateArguments(const std::vector<std::unique_ptr<ExprNode>>& args);
    bool runIteratorStep(const FunctionDeclNode& decl, std::shared_ptr<Environment> closure, Value& element, const std::vector<Value>& extra_args);

    Value makeBoolValue(bool condition);

    //helpers for robot command
    void executeMove(Direction dir);
    void executeTimeshift(const ExprNode* arg_node);
    void executeBind(const ExprNode* arg_node);

    //overriding visitor functions

    //Expression Visitors
    void visit(const NumberNode& node) override;
    void visit(const StringNode& node) override;
    void visit(const VariableNode& node) override;
    void visit(const BinaryOpNode& node) override;
    void visit(const UnaryOpNode& node) override;
    void visit(const CallNode& node) override;
    void visit(const ArrayAccessNode& node) override;

    //Statements Visitors
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