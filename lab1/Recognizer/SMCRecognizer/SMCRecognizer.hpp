#pragma once

#include "IRecognizer.hpp"
#include <unordered_map>

#include "Recognizer_sm.h"

class SMCRecognizer : public IRecognizer
{
private:
    RecognizerContext fsm_;
    std::string buffer_;

    VariableType current_type_;
    std::string current_name_;
    bool is_success_;

    std::unordered_map<std::string, std::vector<VariableType>> symbol_table_;
public:
    SMCRecognizer();
    ~SMCRecognizer() override = default;

    ParseResult parseLine(const std::string& line) override;
    void reset() override;
    std::vector<Variable> getCorrectVariables() const override; 

    //FA actions

    //actions with buffer
    void appendBuffer(char symbol);
    void clearBuffer();
    size_t getBufferSize() const noexcept;

    //working with types
    bool isTypeKeyword() const;
    void setTypeFromBuffer();
    void setDefaultType();

    //wotking with name
    void setVarNameFromBuffer();

    //checking symbol_table
    bool isValidVariableOperand() const;

    //complete
    void completeSuccess();
};