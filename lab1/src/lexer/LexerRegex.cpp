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
        std::regex regex("^" + token.second.regex_);
        std::smatch matches;

        std::string sub = code_.substr(pose_);
        if (std::regex_search(sub, matches, regex))
        {
            Token correct_token{token.second, matches[0].str(), pose_};
            pose_ += matches[0].str().length();

            if (correct_token.type_.name_ != "SPACE")
            {
                tokens_.push_back(correct_token);
            }
            return true;
        }
    }

        std::cout << "Lexer error: Unknown token at pos " << pose_ 
              << ". Character: '" << code_[pose_] << "'" 
              << ". Remaining code: \"" << code_.substr(pose_) << "\"" << std::endl;


    return false;
};
