#include <iostream>
#include "lexer/Lexer.hpp"
#include "common/Recognizer.hpp"

#include <fstream>
#include <sstream>

int main()
{
    std::string filename = "../lab1/src/code.txt"; 
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Bad file: " << filename << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string code = buffer.str();
    file.close();
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