#include "Lexer.hpp"
#include <iostream> //удалить

std::vector<Token> LexerRegex::Run()
{
    while (nextToken()) {}

    return tokens_;
};

bool LexerRegex::nextToken()
{
    if (pose_ >= code_.length()) return false;

    for (auto& token : tokenTypeList)
    {
        //std::cout << "СЧИТЫВАЕМ ПО РЕГУЛЯРКЕ: " << token.second.regex_ << std::endl;
        std::regex regex("^" + token.second.regex_);
        std::smatch matches;

        std::string sub = code_.substr(pose_);
        //std::cout << "СТРОКУ:" <<sub << std::endl;
        if (std::regex_search(sub, matches, regex))
        {
            //std::cout << matches[0] << std::endl;
            Token correct_token{token.second, matches[0].str(), pose_};
            pose_ += matches[0].str().length();

            if (correct_token.type_.name_ != "SPACE")
            {
                tokens_.push_back(correct_token);
            }
            return true;
        }
    }

    return false;
};
