#include <iostream>
#include "lexer/Lexer.hpp"

int main()
{
    std::string code =
    "int var := 5 + 1"
    "short dasha := 10";

    ILexer* lexer = new LexerRegex(code);

    for (auto& it : lexer->Run())
    {
        std::cout << "Token:" << "\n"
        << "'"<< it.name_ << "'"<< "\n"
        << it.type_.name_ << "\n"
        << std::endl;
    }

    delete lexer;
    return 0;
}