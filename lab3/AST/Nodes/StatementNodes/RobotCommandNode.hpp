#pragma once

#include "StatementNode.hpp"
#include <memory>

enum class RobotAction
{
    Top, 
    Bottom, 
    Left, 
    Right, 
    Timeshift, 
    Bind
};

class RobotCommandNode : public StatementNode
{
private:
    RobotAction action_;
    std::unique_ptr<ExprNode> argument_;
public:
    RobotCommandNode(RobotAction action, std::unique_ptr<ExprNode> argument = nullptr)
    : action_(action ),argument_(std::move(argument)) {}

    const RobotAction getAction() const noexcept { return action_; }
    const std::unique_ptr<ExprNode>& getArgument() const noexcept {return argument_;}

    //overriding
    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }

};