#pragma once

#include "TokenType.hpp"
#include "Node.hpp"      
#include <memory>
#include <utility>

struct ParserItem
{
    enum class Type {
        Token,
        Node
    };

    Type type_;
    Token token_;                   
    std::unique_ptr<Node> node_;

    //constructor for tokens
    explicit ParserItem(Token token) : type_(Type::Token), token_(token), node_(nullptr) {}

    //constructor for nodes
    explicit ParserItem(std::unique_ptr<Node> node) : type_(Type::Node), node_(std::move(node)) {}
};