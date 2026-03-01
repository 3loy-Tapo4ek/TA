#pragma once

#include <string>
#include <map>

enum class TypeId
{
    TYPE,
    NAME,
    LIT,
    SIGN,
    ASSIGN
};

struct TokenType
{
    std::string name_;
    std::string regex_;
};

std::map<TypeId, TokenType> tokenTypeList = 
{
    {TypeId::TYPE, TokenType{"TYPE", "\\b(int|short|long)\\b"}},
    {TypeId::NAME, TokenType{"NAME", "[a-z]*"}},
    {TypeId::LIT, TokenType{"LIT", "[0-9]*"}},
    {TypeId::SIGN, TokenType{"SIGN", "{+,-,*,/}"}},
    {TypeId::ASSIGN, TokenType{"ASSIGN", ":="}}
};


