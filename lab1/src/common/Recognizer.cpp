#include "Recognizer.hpp"

std::vector<std::pair<bool, std::string>> Recognizer::TakeStatistics()
{
    std::vector<std::pair<bool, std::string>> output;
    std::string output_string;
    bool check = true;
    size_t current_pose = 0;
    
    for (auto& it : tokens_)
    {
        output_string += it.name_ + " ";
        
        if (current_pose == 0 && IsType(it)) current_pose++;
        else if (current_pose == 1 && IsName(it)) current_pose++;
        else if (current_pose == 2 && IsAssign(it)) current_pose++;
        else if (current_pose == 3 && IsLit(it)) current_pose++;
        else if (current_pose == 4 && IsOperator(it)) current_pose++;
        else if (current_pose == 5 && IsLit(it)) current_pose++;
        else if (!IsDelim(it)) check = false;

        if (IsDelim(it))
        {
            std::pair<bool, std::string> pair(check && current_pose == 6, output_string);
            output.push_back(pair);
            output_string.clear();

            check = true;
            current_pose = 0;
        }
    }

    if (!output_string.empty())
    {
        std::pair<bool, std::string> pair(check && current_pose == 6, output_string);
        output.push_back(pair);
    }
    
    return output;
}

bool Recognizer::IsType(const auto& token) const { return token.type_.name_ == "TYPE"; }
bool Recognizer::IsName(const auto& token) const { return token.type_.name_ == "NAME"; }
bool Recognizer::IsAssign(const auto& token) const { return token.type_.name_ == "ASSIGN"; }
bool Recognizer::IsLit(const auto& token) const { return token.type_.name_ == "LIT"; }
bool Recognizer::IsOperator(const auto& token) const { 
        return token.type_.name_ == "PLUS" || token.type_.name_ == "MINUS" || 
               token.type_.name_ == "MULTIPLY" || token.type_.name_ == "DIV"; 
    }
bool Recognizer::IsDelim(const auto& token) const { return token.type_.name_ == "DELIM"; }