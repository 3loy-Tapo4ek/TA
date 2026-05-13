#pragma once

#include "AST.hpp"
#include "Token.hpp"
#include "ILexer.hpp"
#include <memory>
#include <stdexcept>
#include <string>

namespace regex_engine {

class Parser {
public:
    // Внедрение зависимости (Dependency Injection) через интерфейс
    explicit Parser(ILexer& lexer);

    // Главный метод: парсит все токены и возвращает корень AST
    std::unique_ptr<ASTNode> parse();

private:
    ILexer& lexer_;
    Token currentToken_;

    // Вспомогательные методы
    void advance();
    void consume(TokenType expectedType, const std::string& errorMessage);
    bool isAtPrimaryStart() const;

    // Методы рекурсивного спуска (в порядке возрастания приоритета)
    std::unique_ptr<ASTNode> parseUnion();     // Обработка '|'
    std::unique_ptr<ASTNode> parseConcat();    // Неявная конкатенация (r1 r2)
    std::unique_ptr<ASTNode> parseUnary();     // Обработка '...', '?', '{x}'
    std::unique_ptr<ASTNode> parsePrimary();   // Литералы, скобки и группы
};

class ParseError : public std::runtime_error {
public:
    explicit ParseError(const std::string& message) : std::runtime_error(message) {}
};

} // namespace regex_engine
