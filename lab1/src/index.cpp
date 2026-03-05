#include <iostream>
#include "lexer/Lexer.hpp"
#include "common/Recognizer.hpp"

int main()
{
    std::string code =
    "long var := 10313 + 1\n"
    "int dasha := 10 +\n"
    "int timur := 142 / 2\n"
    "long vova := 228 * 555";

    ILexer* lexer = new LexerRegex(code);
  
    IRecognizer* recognizer = new Recognizer(lexer->Run());

    for (auto& it : recognizer->TakeStatistics())
    {
        std::cout << it.first << " <---> " << it.second << std::endl;
    }

    delete recognizer;
    delete lexer;
    return 0;
}