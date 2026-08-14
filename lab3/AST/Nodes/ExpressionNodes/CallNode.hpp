#pragma once

#include "ExprNode.hpp"
#include <string>
#include <vector>
#include <memory>

class CallNode : public ExprNode
{
private:
    std::string function_name_;
    std::string target_for_;
    std::vector<std::unique_ptr<ExprNode>> arguments_;
public:
    CallNode(std::string fName, std::string targetFor, std::vector<std::unique_ptr<ExprNode>> args)
    : function_name_(std::move(fName)), target_for_(std::move(targetFor)), arguments_(std::move(args)) {}
    
    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }

    const std::string& getFuncName() const noexcept { return function_name_; }
    const std::string& getTarget() const noexcept { return target_for_; }
    const std::vector<std::unique_ptr<ExprNode>>& getArguments() const noexcept {return arguments_; } 
};