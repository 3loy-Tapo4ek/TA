%{
    #include "Tokens.hpp"
%}

%option noyywrap
%option c++
%option case-insensitive
%option never-interactive
%option batch


DIGIT     [0-9]
LETTER    [a-zA-Z]
ID        {LETTER}({LETTER}|{DIGIT}){0,15}
BAD_ID    {LETTER}({LETTER}|{DIGIT}){16,}

%%

[ \t\r\n]+   {}

"int"        { return static_cast<int>(Token::INT); }
"short"      { return static_cast<int>(Token::SHORT); }
"long"       { return static_cast<int>(Token::LONG); }

":="         { return static_cast<int>(Token::ASSIGN); }

"+"          { return static_cast<int>(Token::PLUS); }
"-"          { return static_cast<int>(Token::MINUS); }
"*"          { return static_cast<int>(Token::STAR); }
"/"          { return static_cast<int>(Token::SLASH); }

{DIGIT}+     { return static_cast<int>(Token::NUMBER); }

{BAD_ID}     { return static_cast<int>(Token::ERROR); }
{ID}         { return static_cast<int>(Token::ID); }

.            { return static_cast<int>(Token::ERROR); }

%%