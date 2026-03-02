#pragma once

#include <string>
#include <regex>
#include "common.hpp"

struct Token
{
    TokenType type_;
    std::string name_;
    size_t pos_;
};


