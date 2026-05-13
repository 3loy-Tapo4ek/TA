#include "Parser.hpp"
#include <stdexcept>

namespace regex_engine {

Parser::Parser(ILexer& lexer) : lexer_(lexer) {
    // Инициализируем первый токен
    advance();
}

void Parser::advance() {
    currentToken_ = lexer_.getNextToken();
}

void Parser::consume(TokenType expectedType, const std::string& errorMessage) {
    if (currentToken_.type == expectedType) {
        advance();
    } else {
        throw ParseError(errorMessage);
    }
}

// Проверяет, может ли текущий токен быть началом нового выражения.
// Это необходимо для понимания неявной конкатенации (например, 'a' и 'b' в "ab")
bool Parser::isAtPrimaryStart() const {
    return currentToken_.type == TokenType::LITERAL ||
           currentToken_.type == TokenType::LPAREN ||
           currentToken_.type == TokenType::CAPTURE_START ||
           currentToken_.type == TokenType::NAME_REF;
}

std::unique_ptr<ASTNode> Parser::parse() {
    auto ast = parseUnion();
    
    // После полного разбора должен остаться только маркер конца
    if (currentToken_.type != TokenType::END_OF_FILE) {
        throw ParseError("Unexpected token at the end of expression.");
    }
    
    return ast;
}

// Приоритет 1: Операция ИЛИ (r1 | r2)
std::unique_ptr<ASTNode> Parser::parseUnion() {
    auto left = parseConcat();

    while (currentToken_.type == TokenType::OR) {
        advance(); // Съедаем '|'
        auto right = parseConcat();
        left = std::make_unique<UnionNode>(std::move(left), std::move(right));
    }

    return left;
}

// Приоритет 2: Конкатенация (r1r2)
std::unique_ptr<ASTNode> Parser::parseConcat() {
    auto left = parseUnary();

    // Пока следующий токен может быть началом выражения, мы их конкатенируем
    while (isAtPrimaryStart()) {
        auto right = parseUnary();
        left = std::make_unique<ConcatNode>(std::move(left), std::move(right));
    }

    return left;
}

// Приоритет 3: Унарные операции ('...', '?', '{x}')
std::unique_ptr<ASTNode> Parser::parseUnary() {
    auto node = parsePrimary();

    while (true) {
        if (currentToken_.type == TokenType::KLEENE) {
            advance();
            node = std::make_unique<KleeneNode>(std::move(node));
        } else if (currentToken_.type == TokenType::OPTIONAL) {
            advance();
            node = std::make_unique<OptionalNode>(std::move(node));
        } else if (currentToken_.type == TokenType::REPEAT) {
            int count = std::stoi(currentToken_.value);
            advance();
            node = std::make_unique<RepeatNode>(std::move(node), count);
        } else {
            break; // Нет унарных операторов
        }
    }

    return node;
}

// Приоритет 4: Базовые элементы (литералы, группы)
std::unique_ptr<ASTNode> Parser::parsePrimary() {
    if (currentToken_.type == TokenType::LITERAL) {
        char c = currentToken_.value[0];
        advance();
        return std::make_unique<LiteralNode>(c);
    }
    
    if (currentToken_.type == TokenType::LPAREN) {
        advance(); // Съедаем '('
        auto node = parseUnion(); // Внутри скобок может быть любое выражение
        consume(TokenType::RPAREN, "Expected ')' after expression.");
        return node;
    }
    
    if (currentToken_.type == TokenType::CAPTURE_START) {
        std::string name = currentToken_.value; // Имя уже извлечено лексером
        advance(); // Съедаем '(<name>'
        auto node = parseUnion();
        consume(TokenType::RPAREN, "Expected ')' after capture group expression.");
        return std::make_unique<CaptureGroupNode>(name, std::move(node));
    }
    
    if (currentToken_.type == TokenType::NAME_REF) {
        std::string name = currentToken_.value;
        advance(); // Съедаем '<name>'
        return std::make_unique<NameRefNode>(name);
    }

    throw ParseError("Unexpected token encountered.");
}

} // namespace regex_engine