#pragma once

#include <string>

namespace regex_engine {

    enum class TokenType {
        LITERAL,        // Обычный символ (или экранированный)
        OR,             // |
        KLEENE,         // ...
        OPTIONAL,       // ?
        REPEAT,         // {x}
        LPAREN,         // (
        RPAREN,         // )
        CAPTURE_START,  // (<name>
        NAME_REF,       // <name>
        END_OF_FILE     // Конец выражения
    };

    struct Token {
        TokenType type;
        std::string value; // Хранит символ, число повторений или имя переменной
    };

}
