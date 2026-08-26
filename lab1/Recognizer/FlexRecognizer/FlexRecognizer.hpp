#pragma once

#include "IRecognizer.hpp"
#include "Tokens.hpp"
#include <unordered_map>

#include <FlexLexer.h>
#include <sstream>

class FastStringBuf : public std::streambuf
{
public:
    FastStringBuf(const std::string& str)
    {
        char* ptr = const_cast<char*>(str.data());
        setg(ptr, ptr, ptr + str.size());
    }
};

class FlexRecognizer : public IRecognizer
{
private:
    std::unordered_map<std::string, std::vector<VariableType>> symbol_table_;

    std::string toLowerCase(const std::string& str) const;
    Token nextToken(yyFlexLexer& lexer);

    //helpers
    bool isOperation(Token token) const;
    bool isValidOperand(Token token, const std::string& text) const;
    void registerVariable(const std::string& name, VariableType type);
public:

    ~FlexRecognizer() override = default;

    ParseResult parseLine(const std::string& line) override;
    void reset() override;
    std::vector<Variable> getCorrectVariables() const override; 

};