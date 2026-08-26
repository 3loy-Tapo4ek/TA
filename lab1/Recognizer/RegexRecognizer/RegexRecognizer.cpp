#include "RegexRecognizer.hpp"


RegexRecognizer::RegexRecognizer()
{
    const std::string type_part = R"((?:(int|short|long)\s+)?)";               
    const std::string name_part = R"(([a-zA-Z][a-zA-Z0-9]{0,15}))";
    const std::string operand_part = R"(([a-zA-Z][a-zA-Z0-9]{0,15}|[0-9]+))"; //name or num
    const std::string op_part = R"(([+\-*/]))"; //sign

    const std::string full_pattern = 
        "^\\s*" + 
        type_part + 
        name_part + 
        "\\s*:=\\s*" + 
        operand_part + 
        "\\s*" + op_part + "\\s*" + 
        operand_part + 
        "\\s*$";

    regex_ = std::regex(full_pattern, std::regex_constants::icase | std::regex_constants::optimize);
};

void RegexRecognizer::reset()
{
    symbol_table_.clear();
}

std::vector<Variable> RegexRecognizer::getCorrectVariables() const
{
    std::vector<Variable> result;
    for (const auto& [name, types] : symbol_table_)
    {
        for (VariableType type : types)
        {
            result.push_back(Variable{type, name});
        }
    }
    return result;
}

ParseResult RegexRecognizer::parseLine(const std::string& line)
{
    std::smatch match;

    if (std::regex_match(line, match, regex_))
    {
        std::string operand_1 = match[3].str();
        std::string operand_2 = match[5].str();

        if (!is_valid_operand(operand_1) || !is_valid_operand(operand_2))
        {
            return ParseResult(false, std::nullopt);
        }

        VariableType type = stringToType(match[1].str());
        std::string name = toLowerCase(match[2].str());


        auto& types = symbol_table_[name];
        bool already_exists = false;
        for (VariableType existing_type : types)
        {
            if (existing_type == type)
            {
                already_exists = true;
                break;
            }
        }

        if (!already_exists)
        {
            types.push_back(type);
        }

        Variable var{type, name};
        return ParseResult{true, var};
    }

    return ParseResult{false, std::nullopt};
};

bool RegexRecognizer::is_valid_operand(const std::string& operand)
{
    if (isNumber(operand)) {return true;}

    std::string lower = toLowerCase(operand);
    return symbol_table_.contains(lower);
}

std::string RegexRecognizer::toLowerCase(const std::string& str) const
{
    std::string result = str;
    for (size_t i = 0; i < result.size(); ++i)
    {
        result[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(result[i])));
    }
    return result;
}

bool RegexRecognizer::isNumber(const std::string& str) const
{
    if (str.empty())
    {
        return false;
    }

    for (char c : str)
    {
        if (!std::isdigit(c))
        {
            return false;
        }
    }
    return true;
}

VariableType RegexRecognizer::stringToType(const std::string& str) const
{
    std::string lower = toLowerCase(str);

    if (lower == "short") {return VariableType::Short;}
    if (lower == "long") {return VariableType::Long;}

    return VariableType::Int; 
}
