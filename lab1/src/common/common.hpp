#pragma once

#include <string>
#include <map>

enum class TypeId
{
    TYPE,
    NAME,
    LIT,
    PLUS,
    MINUS,
    MULTIPLY,
    DIV,
    ASSIGN,
    SPACE,
    DELIM
};

struct TokenType
{
    std::string name_;
    std::string regex_;
};

const static std::map<TypeId, TokenType> tokenTypeList = 
{
    {TypeId::TYPE, TokenType{"TYPE", "(int|short|long)"}},
    {TypeId::NAME, TokenType{"NAME", "[a-z]+"}},
    {TypeId::LIT, TokenType{"LIT", "(\\d+)"}},
    {TypeId::PLUS, TokenType{"PLUS", "\\+"}},
    {TypeId::MINUS, TokenType{"MINUS", "\\-"}},
    {TypeId::MULTIPLY, TokenType{"MULTIPLY", "\\*"}},
    {TypeId::DIV, TokenType{"DIV", "\\/"}},
    {TypeId::ASSIGN, TokenType{"ASSIGN", "(:=)"}},
    {TypeId::SPACE, TokenType{"SPACE", "([ \\t]+)"}},
    {TypeId::DELIM, TokenType{"DELIM", "(\\n)"}}
};

struct Stats
{
    std::string buffer_;
    std::vector<std::pair<bool, std::string>> output_;
};