#include "SMCRecognizer.hpp"

SMCRecognizer::SMCRecognizer()
    :fsm_(*this), current_type_(VariableType::Int), is_success_(false) {}

ParseResult SMCRecognizer::parseLine(const std::string& line)
{

    is_success_ = false;
    clearBuffer();
    fsm_.enterStartState();

    for (char symbol : line)
    {

        if (fsm_.getState().getId() == MainMap::Error.getId())
        {
            break;
        }

        if (std::isspace(symbol))
        {
            fsm_.Space();
        }
        else if (std::isalpha(symbol))
        {
            fsm_.Letter(symbol);
        }
        else if (std::isdigit(symbol))
        {
            fsm_.Digit(symbol);
        }
        else if (symbol == ':')
        {
            fsm_.Colon();
        }
        else if (symbol == '=')
        {
            fsm_.Equal();
        }
        else if (symbol == '+' || symbol == '-' || symbol == '*' || symbol == '/')
        {
            fsm_.Op(symbol);
        }
        else
        {
            fsm_.Unknown();
        }
    }

    fsm_.EOL();

    if (is_success_)
    {
        return {true, Variable{current_type_, current_name_}};
    }
    return {false, std::nullopt};
}

std::vector<Variable> SMCRecognizer::getCorrectVariables() const
{
    std::vector<Variable> result;
    for (const auto& [name, types] : symbol_table_)
    {
        for (VariableType type : types)
        {
            result.push_back(Variable{type, name});
        }
    }
    return result;
};

void SMCRecognizer::reset()
{
    symbol_table_.clear();
};

void SMCRecognizer::clearBuffer()
{
    buffer_.clear();
};

void SMCRecognizer::appendBuffer(char symbol)
{
    buffer_.push_back(std::tolower(symbol));
}

size_t SMCRecognizer::getBufferSize() const noexcept
{
    return buffer_.size();
}

bool SMCRecognizer::isTypeKeyword() const
{
    return (buffer_ == "int" || buffer_ == "short" || buffer_ == "long");
}

void SMCRecognizer::setTypeFromBuffer()
{
    if (buffer_ == "int") {current_type_ = VariableType::Int;}
    if (buffer_ == "short") {current_type_ = VariableType::Short;}
    if (buffer_ == "long") {current_type_ = VariableType::Long;}
}

void SMCRecognizer::setDefaultType()
{
    current_type_ = VariableType::Int;
}

void SMCRecognizer::setVarNameFromBuffer()
{
    current_name_ = buffer_;
}

bool SMCRecognizer::isValidVariableOperand() const
{
    return symbol_table_.contains(buffer_);
}

void SMCRecognizer::completeSuccess()
{
    auto& types = symbol_table_[current_name_];
    bool already_exists = false;

    for (VariableType existing_type : types)
    {
        if (existing_type == current_type_)
        {
            already_exists = true;
            break;
        }
    }

    if (!already_exists)
    {
        types.push_back(current_type_);
    }
    is_success_ = true;
}
