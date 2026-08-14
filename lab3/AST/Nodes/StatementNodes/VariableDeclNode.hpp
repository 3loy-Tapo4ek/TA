#pragma once

#include "StatementNode.hpp"
#include "ExprNode.hpp"
#include <string>
#include <memory>


class VariableDeclNode : public StatementNode
{
private:
    DataType type_;
    std::string name_;

    bool is_mutable_;

    std::unique_ptr<ExprNode> initial_value_;
    std::unique_ptr<ExprNode> array_size_;
public:
    VariableDeclNode(DataType type, std::string name, bool isMutable, std::unique_ptr<ExprNode> initialValue, std::unique_ptr<ExprNode> arraySize)
    : type_(type), name_(std::move(name)), is_mutable_(isMutable),
      initial_value_(std::move(initialValue)), array_size_(std::move(arraySize)) {}

    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }

    const DataType getType() const noexcept { return type_; }
    const std::string& getName() const noexcept { return name_; }
    const bool isMutable() const noexcept { return is_mutable_; }
    const std::unique_ptr<ExprNode>& getInitialValue() const noexcept { return initial_value_;}
    const std::unique_ptr<ExprNode>& getArraySize() const noexcept { return array_size_;}
};

