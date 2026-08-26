#pragma once

#include "IRecognizer.hpp"
#include <regex>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <algorithm>
 
class RegexRecognizer : public IRecognizer
{
private:
    std::regex regex_;

    std::unordered_map<std::string, std::vector<VariableType>> symbol_table_;

    VariableType stringToType(const std::string& string) const;
    std::string toLowerCase(const std::string& str) const;
    bool isNumber(const std::string& str) const;

    bool is_valid_operand(const std::string& operand);
public:
    RegexRecognizer();
    ~RegexRecognizer() override = default;

    ParseResult parseLine(const std::string& line) override;
    void reset() override;
    std::vector<Variable> getCorrectVariables() const override; 
};