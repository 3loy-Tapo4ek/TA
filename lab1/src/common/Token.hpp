#pragma once

#include <string>
#include <regex>
#include "common.hpp"

class Token
{
private:
    TokenType type_;
    std::string name_;

    size_t pos_;
public:
    Token(TokenType type, std::string name, size_t pos) 
    : type_(type), name_(name), pos_(pos) {}
};


