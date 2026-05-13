#pragma once

#include "Token.hpp"

namespace regex_engine {

class ILexer {
public:
    virtual ~ILexer() = default;
    
    // Возвращает следующий токен из потока
    virtual Token getNextToken() = 0;
};

} // namespace regex_engine
