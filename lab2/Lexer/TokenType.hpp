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
    Repeat,      // {x} x - число
    NamedGroupName, // (<name>r) (метасимвол ‘(<name>)’, name – имя группы захвата)
    NamedGroupRef   // <name>’, name – имя группы захвата

};

struct Token
{
    TokenType token_type_;
    std::optional<char> value;
    std::optional<size_t> repeat_value;
    std::optional<std::string> group_name;
};
