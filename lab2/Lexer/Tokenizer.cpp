#include "Tokenizer.hpp"

std::vector<Token> Tokenizer::tokenize(std::string input_value)
{
    std::vector<Token> output_tokens;

    auto ptr = input_value.cbegin();

    while (ptr != input_value.cend())
    {
        //screeninig
        if (*ptr == '%')
        {
            auto [start_ptr, end_ptr] = findEscapedBlock(ptr, input_value.cend(), '%');
            for (auto inner_it = start_ptr; inner_it != end_ptr; ++inner_it)
            {
                output_tokens.push_back(Token{TokenType::Literal, *inner_it});
            }

            ptr = end_ptr;
        }

        //basic symbols

        else if (*ptr == '|')
        {
            output_tokens.push_back(Token{TokenType::Alternation});
        }

        else if (input_value.cend() - ptr >= 3 && *ptr == '.' && *(ptr+1) == '.' && *(ptr+2) == '.')
        {
            output_tokens.push_back(Token{TokenType::KleeneStar});
            ptr += 2;
        }

        else if (*ptr == '(')
        {
            output_tokens.push_back(Token{TokenType::OpenParen});
        }

        else if (*ptr == ')')
        {
            output_tokens.push_back(Token{TokenType::CloseParen});
        }

        //here comes crazy stuff

        else if (*ptr == '?')
        {
            output_tokens.push_back(Token{TokenType::Question});
        }

        else if (*ptr == '{')
        {
            auto [start_ptr, end_ptr] = findEscapedBlock(ptr, input_value.cend(), '}');

            if (std::all_of(start_ptr, end_ptr, ::isdigit))
            {
                size_t count = std::stoul(std::string(start_ptr, end_ptr));
                output_tokens.push_back(Token{TokenType::Repeat, std::nullopt, count});
            }
            ptr = end_ptr;
        }
        else
        {
            output_tokens.push_back(Token{TokenType::Literal, *ptr});
        }

        ptr++;
    }

    return output_tokens;
};

std::pair<std::string::const_iterator, std::string::const_iterator> Tokenizer::findEscapedBlock(std::string::const_iterator it, std::string::const_iterator end, char closing)
{
    ++it; //skip opening symbol
        
    if (it == end) 
    {
        throw std::runtime_error("Некорректное экранирование в конце строки");
    }

    auto start_it = it;
    ++it; //skip first symbol

    //search for closing %
    while (it != end && *it != closing)
    {
        ++it;
    }

    if (it == end) 
    {
        throw std::runtime_error("Забыли закрыть экранирование '%'");
    }

    return {start_it, it};
};

