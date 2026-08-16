%{
    #include <string>
    #include "parser.tab.hh"

    #define YY_DECL yy::parser::symbol_type yylex()
    #define YY_USER_ACTION location.columns(yyleng);
%}

%option noyywrap

%{
    yy::parser::location_type location;
%}

/* white space */
WS [ \t\r]+
/* newline */
NEWLINE \n
/* ID */
ID [a-zA-Z_][a-zA-Z0-9_]*
/* integer literals */
INT [0-9]+
/* string literals */
STRING \"[^\"]*\"


/* RULES RULES RULES RULES RULES RULES RULES RULES RULES */   
%%  

%{
    location.step();
%}

    /* handle whitespace and newlines */
{WS} {location.step();}
{NEWLINE} {location.lines(yyleng); location.step();}

    /* handle TYPES and MODIFICATORS */
"mutable" {return yy::parser::make_MUTABLE(location);}
"integer" {return yy::parser::make_INTEGER(location);}
"string" {return yy::parser::make_STRING(location);}
"pointer" {return yy::parser::make_POINTER(location);}
"array" {return yy::parser::make_ARRAY(location);}
"of" {return yy::parser::make_OF(location);}

    /* handle BLOCKS */
"start" {return yy::parser::make_START(location);}
"finish" {return yy::parser::make_FINISH(location);}
"while" {return yy::parser::make_WHILE(location);}
"instead" {return yy::parser::make_INSTEAD(location);}
"checkzero" {return yy::parser::make_CHECKZERO(location);}
"break" {return yy::parser::make_BREAK(location);}

    /* handle FUNCTIONS and ITERATORS */
"call" {return yy::parser::make_CALL(location);}
"for" {return yy::parser::make_FOR(location);}
"with" {return yy::parser::make_WITH(location);}
"as" {return yy::parser::make_AS(location);}
"iterator" {return yy::parser::make_ITERATOR(location);}
"return" {return yy::parser::make_RETURN(location);}
"current" {return yy::parser::make_CURRENT(location);}
"last" {return yy::parser::make_LAST(location);}

    /* handle ROBOT commands */
"top" {return yy::parser::make_TOP(location);}
"bottom" {return yy::parser::make_BOTTOM(location);}
"left" {return yy::parser::make_LEFT(location);}
"right" {return yy::parser::make_RIGHT(location);}
"timeshift" {return yy::parser::make_TIMESHIFT(location);}
"bind" {return yy::parser::make_BIND(location);}

    /* handle OPARATORS and SEPARATORS */
":=" {return yy::parser::make_ASSIGN(location);}

"+" {return yy::parser::make_PLUS(location);}
"-" {return yy::parser::make_MINUS(location);}
"/" {return yy::parser::make_SLASH(location);}
"%" {return yy::parser::make_PERCENT(location);}

"*" {return yy::parser::make_STAR(location);}
"&" {return yy::parser::make_AMPERSAND(location);}
"?" {return yy::parser::make_QUESTION(location);}

"(" {return yy::parser::make_OPENPAREN(location);}
")" {return yy::parser::make_CLOSEPAREN(location);}
"[" {return yy::parser::make_OPENBRACKET(location);}
"]" {return yy::parser::make_CLOSEBRACKET(location);}
"," {return yy::parser::make_COMMA(location);}
";" {return yy::parser::make_SEMICOLON(location);}

"=" {return yy::parser::make_EQUAL(location);}
"<" {return yy::parser::make_LESS(location);}
">" {return yy::parser::make_GREATER(location);}

    /* Literals and ID */
{INT}    { return yy::parser::make_INT_LITERAL(std::stoll(yytext), location); }
{STRING} { 
            std::string string(yytext);
            return yy::parser::make_STRING_LITERAL(string.substr(1, string.length() - 2), location); 
         }
{ID}     { return yy::parser::make_IDENTIFIER(std::string(yytext), location); }

.        { throw yy::parser::syntax_error(location, "Unknown character: " + std::string(yytext)); }

<<EOF>> {return yy::parser::make_YYEOF(location);}
%%