#pragma once

#include <string>
#include <optional>

enum class TokenType 
{
    Literal,
    Alternation, // |
    KleeneStar,  // ...
    OpenParen,   // (
    CloseParen   // )
};

struct Token
{
    TokenType token_type_;
    std::optional<char> value;
};
