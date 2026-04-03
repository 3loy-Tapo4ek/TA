#include "Lexer.hpp"
#include <iostream>
#include <vector>
#include <regex>

static std::vector<std::pair<TokenType, std::regex>> get_compiled_regexes() {
    static std::vector<std::pair<TokenType, std::regex>> compiled;
    if (compiled.empty()) {
        for (const auto& token : tokenTypeList) {
            compiled.push_back({token.second, std::regex(token.second.regex_)});
        }
    }
    return compiled;
}

std::vector<Token> LexerRegex::Run()
{
    while (nextToken()) {}
    return tokens_;
}

bool LexerRegex::nextToken()
{
    if (pose_ >= code_.length()) return false;

    const auto& compiled_regexes = get_compiled_regexes();

    auto start_it = code_.cbegin() + pose_;
    auto end_it = code_.cend();

    for (const auto& rx_pair : compiled_regexes)
    {
        std::smatch matches;

        if (std::regex_search(start_it, end_it, matches, rx_pair.second, std::regex_constants::match_continuous))
        {
            std::string match_str = matches[0].str();
            Token correct_token{rx_pair.first, match_str, pose_};
            
            pose_ += match_str.length();

            if (correct_token.type_.name_ != "SPACE")
            {
                tokens_.push_back(correct_token);
            }
            return true;
        }
    }

    std::cerr << "Lexer error: Unknown token at pos " << pose_ 
              << ". Character: '" << code_[pose_] << "'\n";

    return false;
}