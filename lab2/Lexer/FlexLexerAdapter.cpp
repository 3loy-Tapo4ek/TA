#include "FlexLexerAdapter.hpp"

namespace regex_engine {

FlexLexerAdapter::FlexLexerAdapter(std::istream& input) 
    : lexer_(std::make_unique<yyFlexLexer>(&input)) {}

Token FlexLexerAdapter::getNextToken() {
    // 1. Получаем тип токена от Flex
    int type_code = lexer_->yylex();
    TokenType type = static_cast<TokenType>(type_code);
    
    // 2. Получаем сырой текст совпадения
    std::string raw_text = lexer_->YYText() ? lexer_->YYText() : "";
    std::string value;

    // 3. Форматируем значение (чистим от лишних символов)
    switch (type) {
        case TokenType::LITERAL:
            // Обработка экранирования %s% -> s
            if (raw_text.length() >= 3 && raw_text.front() == '%' && raw_text.back() == '%') {
                value = std::string(1, raw_text[1]);
            } else {
                value = raw_text; // Обычный символ
            }
            break;
        case TokenType::REPEAT:
            // {5} -> 5
            value = raw_text.substr(1, raw_text.length() - 2);
            break;
        case TokenType::CAPTURE_START:
            // (<name> -> name
            value = raw_text.substr(2, raw_text.length() - 3);
            break;
        case TokenType::NAME_REF:
            // <name> -> name
            value = raw_text.substr(1, raw_text.length() - 2);
            break;
        case TokenType::END_OF_FILE:
            value = "";
            break;
        default:
            // Для операторов значение можно оставить как есть
            value = raw_text;
            break;
    }

    return Token{type, value};
}

} // namespace regex_engine