#pragma once

#include <string>
#include <vector>
#include "TokenType.hpp"
#include "stdexcept"
#include <algorithm>


class Tokenizer
{
private:
    std::pair<std::string::const_iterator, std::string::const_iterator> findEscapedBlock(std::string::const_iterator it, std::string::const_iterator end, char closing);
public:
    Tokenizer() {};
    ~Tokenizer() = default;

    std::vector<Token> tokenize(std::string input_value);
};