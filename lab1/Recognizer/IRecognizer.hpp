#pragma once

#include <string>
#include <vector>
#include <optional>

enum class VariableType
{
    Int,
    Short,
    Long
};

inline const std::string typeToString(VariableType type) noexcept
{
    switch (type)
    {
        case VariableType::Int:   return "int";
        case VariableType::Short: return "short";
        case VariableType::Long:  return "long";
    }

    return "int";
}

struct Variable
{
    VariableType type_;
    std::string name_;

    auto operator<=>(const Variable&) const = default;
};

using ParseResult = std::pair<bool, std::optional<Variable>>;

class IRecognizer
{
public:
    virtual ~IRecognizer() = default;

    virtual ParseResult parseLine(const std::string& line) = 0;
    virtual void reset() = 0;
    virtual std::vector<Variable> getCorrectVariables() const = 0; 
};
