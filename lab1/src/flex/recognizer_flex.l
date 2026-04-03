%{
#include "Token.hpp"
#include "common.hpp"
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

std::vector<Token>* flex_tokens_out = nullptr;
std::size_t flex_pos = 0;

static void emit_token(const char* typename_, const char* text, int len)
{
    if (!flex_tokens_out)
        return;
    TokenType tt{};
    for (const auto& entry : tokenTypeList)
    {
        if (entry.second.name_ == typename_)
        {
            tt = entry.second;
            break;
        }
    }
    flex_tokens_out->emplace_back(Token{tt, std::string(text, static_cast<std::size_t>(len)), flex_pos});
    flex_pos += static_cast<std::size_t>(len);
}

%}

%option noyywrap
%option nounput
%option noinput

%%

[ \t]+                  { flex_pos += static_cast<std::size_t>(yyleng); }

\n                      { emit_token("DELIM", yytext, yyleng); }

":="                    { emit_token("ASSIGN", yytext, yyleng); }

"*"                     { emit_token("MULTIPLY", yytext, yyleng); }

"+"                     { emit_token("PLUS", yytext, yyleng); }

"-"                     { emit_token("MINUS", yytext, yyleng); }

"/"                     { emit_token("DIV", yytext, yyleng); }

"int"|"short"|"long"    { emit_token("TYPE", yytext, yyleng); }

[0-9]+                  { emit_token("LIT", yytext, yyleng); }

[a-z]+                  { emit_token("NAME", yytext, yyleng); }

.                       {
    std::cerr << "Lexer error: Unknown token at pos " << flex_pos
              << ". Character: '" << yytext[0] << "'" << std::endl;
    return 0;
}

%%
