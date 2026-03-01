#pragma once

#include <string>
#include <regex>

class ILexer
{
public:
    virtual ~ILexer();
};

class LexerRegex : public ILexer
{
private:
    std::string code_;
public:
    LexerRegex(std::string code) : code_(code) {}

    bool 
};