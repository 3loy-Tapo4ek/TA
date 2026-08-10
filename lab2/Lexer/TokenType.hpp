#pragma once

#include <string>
#include <optional>

enum class TokenType 
{
    //some basic metasymbols
    Literal,
    Alternation, // |
    KleeneStar,  // ...
    OpenParen,   // (
    CloseParen,  // )
    //some crazy stuff
    Question,    // ?
    Repeat       // {x} x - число

};

struct Token
{
    TokenType token_type_;
    std::optional<char> value;
    std::optional<size_t> repeat_value;
};
