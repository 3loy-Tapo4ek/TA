%option c++
%option noyywrap

%{
#include "Token.hpp"
using namespace regex_engine;
%}

DIGIT       [0-9]
IDENTIFIER  [a-zA-Z_][a-zA-Z0-9_]*

%%

"%".[ \t\n]*"%"     { return static_cast<int>(TokenType::LITERAL); }
"|"                 { return static_cast<int>(TokenType::OR); }
"..."               { return static_cast<int>(TokenType::KLEENE); }
"?"                 { return static_cast<int>(TokenType::OPTIONAL); }
"{"{DIGIT}+"}"      { return static_cast<int>(TokenType::REPEAT); }
"(<"{IDENTIFIER}">" { return static_cast<int>(TokenType::CAPTURE_START); }
"<"{IDENTIFIER}">"  { return static_cast<int>(TokenType::NAME_REF); }
"("                 { return static_cast<int>(TokenType::LPAREN); }
")"                 { return static_cast<int>(TokenType::RPAREN); }
<<EOF>>             { return static_cast<int>(TokenType::END_OF_FILE); }
.                   { return static_cast<int>(TokenType::LITERAL); }

%%