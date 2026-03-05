#pragma once

#include <utility>
#include <string>
#include <vector>

#include "Token.hpp"



class IRecognizer
{
public:
    virtual std::vector<std::pair<bool, std::string>> TakeStatistics() = 0;

    virtual ~IRecognizer() {};
};

class Recognizer : public IRecognizer
{
private:
    std::vector<Token> tokens_;
    size_t pose_ = 0;

    bool IsType(const auto& token) const;
    bool IsName(const auto& token) const;
    bool IsAssign(const auto& token) const;
    bool IsLit(const auto& token) const;
    bool IsOperator(const auto& token) const;
    bool IsDelim(const auto& token) const;
public:
    Recognizer(std::vector<Token> tokens) : tokens_(tokens) {}

    std::vector<std::pair<bool, std::string>> TakeStatistics() override;

    ~Recognizer() override = default;
};