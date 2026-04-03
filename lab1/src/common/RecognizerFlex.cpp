#include "Recognizer.hpp"

#include <string>
#include <utility>
#include <vector>

extern std::vector<Token>* flex_tokens_out;
extern std::size_t flex_pos;

typedef struct yy_buffer_state* YY_BUFFER_STATE;
YY_BUFFER_STATE yy_scan_string(const char* str);
void yy_delete_buffer(YY_BUFFER_STATE buffer);
int yylex(void);

void RecognizerFlex::LexWithFlex(const std::string& source)
{
    tokens_.clear();
    flex_tokens_out = &tokens_;
    flex_pos = 0;

    YY_BUFFER_STATE buf = yy_scan_string(source.c_str());
    yylex();
    yy_delete_buffer(buf);

    flex_tokens_out = nullptr;
}

RecognizerFlex::RecognizerFlex(const std::string& source)
{
    LexWithFlex(source);
}

std::vector<std::pair<bool, std::string>> RecognizerFlex::TakeStatistics()
{
    std::vector<std::pair<bool, std::string>> output;
    std::string output_string;
    bool check = true;
    size_t current_pose = 0;

    for (const auto& it : tokens_)
    {
        output_string += it.name_ + " ";

        if (current_pose == 0 && IsType(it))
            current_pose++;
        else if (current_pose == 1 && IsName(it))
            current_pose++;
        else if (current_pose == 2 && IsAssign(it))
            current_pose++;
        else if (current_pose == 3 && IsLit(it))
            current_pose++;
        else if (current_pose == 4 && IsOperator(it))
            current_pose++;
        else if (current_pose == 5 && IsLit(it))
            current_pose++;
        else if (!IsDelim(it))
            check = false;

        if (IsDelim(it))
        {
            if (!output_string.empty())
                output_string.pop_back();

            output.push_back({check && current_pose == 6, output_string});
            output_string.clear();
            check = true;
            current_pose = 0;
        }
    }

    if (!output_string.empty())
    {
        output_string.pop_back();
        output.push_back({false, output_string}); 
    }

    return output;
}

bool RecognizerFlex::IsType(const Token& token) const { return token.type_.name_ == "TYPE"; }
bool RecognizerFlex::IsName(const Token& token) const { return token.type_.name_ == "NAME"; }
bool RecognizerFlex::IsAssign(const Token& token) const { return token.type_.name_ == "ASSIGN"; }
bool RecognizerFlex::IsLit(const Token& token) const { return token.type_.name_ == "LIT"; }
bool RecognizerFlex::IsOperator(const Token& token) const
{
    return token.type_.name_ == "PLUS" || token.type_.name_ == "MINUS" ||
           token.type_.name_ == "MULTIPLY" || token.type_.name_ == "DIV";
}
bool RecognizerFlex::IsDelim(const Token& token) const { return token.type_.name_ == "DELIM"; }