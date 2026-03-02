#pragma once

#include <string>
#include <vector>
#include <regex>

#include "../common/Token.hpp"

class ILexer
{
public:
    virtual std::vector<Token> Run() = 0;

    virtual ~ILexer() {};
};

class LexerRegex : public ILexer
{
private:
    std::string code_;
    size_t pose_ = 0;
    std::vector<Token> tokens_;

    bool nextToken();
public:
    LexerRegex(std::string code) : code_(code) {}

    std::vector<Token> Run() override;

    ~LexerRegex() override = default;
};