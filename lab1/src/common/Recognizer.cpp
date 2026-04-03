#include "Recognizer.hpp"

#include <regex>
#include <sstream>

namespace
{
const char* kLinePattern = R"(^\s*(int|short|long)\s+[a-z]+\s*:=\s*\d+\s*[-+*/]\s*\d+\s*$)";
}

std::vector<std::pair<bool, std::string>> Recognizer::TakeStatistics()
{
    static const std::regex line_ok(kLinePattern, std::regex::ECMAScript);
    std::vector<std::pair<bool, std::string>> output;
    std::istringstream in(source_);
    std::string line;

    while (std::getline(in, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        std::string display = line;
        if (!display.empty() && display.back() != ' ')
            display += ' ';

        const bool ok = std::regex_match(line, line_ok);
        output.push_back({ok, display});
    }

    return output;
}

bool Recognizer::IsType(const Token& token) const
{
    (void)token;
    return false;
}
bool Recognizer::IsName(const Token& token) const
{
    (void)token;
    return false;
}
bool Recognizer::IsAssign(const Token& token) const
{
    (void)token;
    return false;
}
bool Recognizer::IsLit(const Token& token) const
{
    (void)token;
    return false;
}
bool Recognizer::IsOperator(const Token& token) const
{
    (void)token;
    return false;
}
bool Recognizer::IsDelim(const Token& token) const
{
    (void)token;
    return false;
}
