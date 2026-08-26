#pragma once

#include "StatementNode.hpp"
#include <string>
#include <memory>
#include <vector>
#include <utility>


class FunctionDeclNode : public StatementNode
{
private:
    DataType return_type_;
    std::string name_;

    std::vector<std::pair<DataType, std::string>> parameters_;

    bool is_iterator_;
    bool is_mutable_iterator_;

    DataType iterator_for_type_;

    std::unique_ptr<StatementNode> body_;
public:
    FunctionDeclNode(DataType type, std::string name, std::vector<std::pair<DataType, std::string>> parameters,
                     bool isIterator, bool isMutableIterator, DataType iteratorType,
                     std::unique_ptr<StatementNode> body)
    : return_type_(type), name_(std::move(name)), parameters_(std::move(parameters)),
      is_iterator_(isIterator),
      is_mutable_iterator_(std::move(isMutableIterator)), iterator_for_type_(iteratorType),
      body_(std::move(body)) {}

    void accept(INodeVisitor& visitor) const override { visitor.visit(*this); }

    const DataType getType() const noexcept { return return_type_; }
    const std::string& getName() const noexcept { return name_; }
    const std::vector<std::pair<DataType, std::string>>& getParameters() const noexcept {return parameters_;}

    const bool isIterator() const noexcept { return is_iterator_; }
    const bool isMutableIterator() const noexcept { return is_mutable_iterator_; }

    const DataType getIteratorType() const noexcept { return iterator_for_type_;}
    const std::unique_ptr<StatementNode>& getBody() const noexcept { return body_;}
};

