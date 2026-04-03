#pragma once

#include <utility>
#include <string>
#include <vector>

#include "Token.hpp"
#include "../smc/RecognizerSMC_sm.h"

class IRecognizer
{
public:
    virtual std::vector<std::pair<bool, std::string>> TakeStatistics() = 0;

    virtual bool IsType(const Token& token) const = 0;
    virtual bool IsName(const Token& token) const = 0;
    virtual bool IsAssign(const Token& token) const = 0;
    virtual bool IsLit(const Token& token) const = 0;
    virtual bool IsOperator(const Token& token) const = 0;
    virtual bool IsDelim(const Token& token) const = 0;

    virtual ~IRecognizer() {};
};

class Recognizer : public IRecognizer
{
private:
    std::string source_;

    bool IsType(const Token& token) const override;
    bool IsName(const Token& token) const override;
    bool IsAssign(const Token& token) const override;
    bool IsLit(const Token& token) const override;
    bool IsOperator(const Token& token) const override;
    bool IsDelim(const Token& token) const override;

public:
    explicit Recognizer(std::string source) : source_(std::move(source)) {}

    std::vector<std::pair<bool, std::string>> TakeStatistics() override;

    ~Recognizer() override = default;
};

class RecognizerSMC : public IRecognizer
{
private:
    RecognizerSMCContext context_;
    
    size_t pose_ = 0;
    std::vector<Token> tokens_;

public:
    RecognizerSMC(std::vector<Token> tokens) : context_(*this), tokens_(tokens) {}

    bool IsType(const Token& token) const override;
    bool IsName(const Token& token) const override;
    bool IsAssign(const Token& token) const override;
    bool IsLit(const Token& token) const override;
    bool IsOperator(const Token& token) const override;
    bool IsDelim(const Token& token) const override;

    std::vector<std::pair<bool, std::string>> TakeStatistics() override;


    void pose_increment(const Token& token, Stats& stats);
    void alarm(const Token& token, Stats& stats);
    void pose_restart(const Token& token, Stats& stats);
    void restart_from_error(const Token& token, Stats& stats);

    ~RecognizerSMC() override = default;
};

class RecognizerFlex : public IRecognizer
{
private:
    std::vector<Token> tokens_;

    void LexWithFlex(const std::string& source);

    bool IsType(const Token& token) const override;
    bool IsName(const Token& token) const override;
    bool IsAssign(const Token& token) const override;
    bool IsLit(const Token& token) const override;
    bool IsOperator(const Token& token) const override;
    bool IsDelim(const Token& token) const override;

public:
    RecognizerFlex(const std::string& source);


    std::vector<std::pair<bool, std::string>> TakeStatistics() override;

    ~RecognizerFlex() override = default;
};