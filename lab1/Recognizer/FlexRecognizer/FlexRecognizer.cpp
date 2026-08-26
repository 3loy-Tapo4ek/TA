#include "FlexRecognizer.hpp"

bool FlexRecognizer::isOperation(Token token) const
{
    return token == Token::PLUS || token == Token::MINUS || 
           token == Token::STAR || token == Token::SLASH;
}

bool FlexRecognizer::isValidOperand(Token token, const std::string& text) const
{
    if (token == Token::NUMBER)
    {
        return true;
    }
    if (token == Token::ID)
    {
        return symbol_table_.contains(toLowerCase(text));
    }
    return false;
}

void FlexRecognizer::registerVariable(const std::string& name, VariableType type)
{
    auto& types = symbol_table_[name];
    for (VariableType existing_type : types)
    {
        if (existing_type == type)
        {
            return;
        }
    }
    types.push_back(type);
}

void FlexRecognizer::reset()
{
    symbol_table_.clear();
};

std::vector<Variable> FlexRecognizer::getCorrectVariables() const
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

ParseResult FlexRecognizer::parseLine(const std::string& line)
{
    FastStringBuf buf(line);
    std::istream stream(&buf);
    yyFlexLexer lexer(&stream);

    Token current = nextToken(lexer);

    //optional type
    VariableType type = VariableType::Int;
    if (current == Token::INT || current == Token::SHORT || current == Token::LONG)
    {
        if (current == Token::SHORT) { type = VariableType::Short; }
        else if (current == Token::LONG) { type = VariableType::Long; }
        current = nextToken(lexer);
    }

    //variable name
    if (current != Token::ID) { return {false, std::nullopt}; }
    std::string lhs_name = toLowerCase(lexer.YYText());
    
    //assign ':='
    if (nextToken(lexer) != Token::ASSIGN) {return {false, std::nullopt};}

    //operand_1
    current = nextToken(lexer);
    if (!isValidOperand(current, lexer.YYText())) {return {false, std::nullopt};}

    //operation sign
    if (!isOperation(nextToken(lexer))) {return {false, std::nullopt};}

    //operand_2
    current = nextToken(lexer);
    if (!isValidOperand(current, lexer.YYText())) {return {false, std::nullopt};}

    //end of line
    if (nextToken(lexer) != Token::END_OF_FILE) {return {false, std::nullopt};}

    registerVariable(lhs_name, type);
    return {true, Variable{type, lhs_name}};
}

Token FlexRecognizer::nextToken(yyFlexLexer& lexer)
{
    return static_cast<Token>(lexer.yylex());
}


std::string FlexRecognizer::toLowerCase(const std::string& str) const
{
    std::string result = str;
    for (size_t i = 0; i < result.size(); ++i)
    {
        result[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(result[i])));
    }
    return result;
}