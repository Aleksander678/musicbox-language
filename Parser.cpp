#include "Parser.h"
#include "AST.h"
#include "Token.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> parsedStatements;

    while (!isAtEnd()) {
        while (match(TokenType::Newline)) {}
        if (isAtEnd()) break;
        parsedStatements.push_back(parseStatement());
    }

    return parsedStatements;
}

std::unique_ptr<Stmt> Parser::parseStatement() {
    if (match(TokenType::Let)) {
        return parseLetStatement();
    } 
    if (match(TokenType::Fun)) {
        return parseFunctionStatement();
    }
    if (match(TokenType::Return)) {
        return parseReturnStatement();
    }

    advance();
    return nullptr;
}

std::unique_ptr<Stmt> Parser::parseLetStatement() {
    Token nameToken = advance();
    std::string varName = nameToken.value;
    
    match(TokenType::Equals);

    std::unique_ptr<Expr> initializer = parseExpression();
    match(TokenType::Newline);

    return std::make_unique<LetStmt>(varName, std::move(initializer));
}

std::unique_ptr<Stmt> Parser::parseFunctionStatement() {
    Token nameToken = advance();
    match(TokenType::OpenParen);

    std::vector<std::string> parameters;

    if (peek().type != TokenType::CloseParen) {
        do {
            Token paramToken = advance();
            parameters.push_back(paramToken.value);
        } while (match(TokenType::Comma));
    }

    match(TokenType::CloseParen); 
    match(TokenType::OpenBrace);

    std::vector<std::unique_ptr<Stmt>> body = parseBlock();

    return std::make_unique<FunctionStmt>(nameToken.value, std::move(parameters), std::move(body));
}

std::unique_ptr<Stmt> Parser::parseReturnStatement() {
    std::unique_ptr<Expr> value = parseExpression();
    match(TokenType::Newline);
    return std::make_unique<ReturnStmt>(std::move(value));
}

std::unique_ptr<Expr> Parser::parseExpression() {
    return parseTerm();
}

std::unique_ptr<Expr> Parser::parsePrimary() {
    if (match(TokenType::Number)) {
        return std::make_unique<LiteralExpr>(previous().value);
    }
    if (match(TokenType::String)) {
        return std::make_unique<LiteralExpr>(previous().value);
    }
    if (match(TokenType::Identifier)) {
        return std::make_unique<VariableExpr>(previous().value);
    }
    if (match(TokenType::OpenParen)) {
        std::unique_ptr<Expr> expr = parseExpression(); 
        match(TokenType::CloseParen); 
        return expr; 
    }

    return nullptr;
}

std::unique_ptr<Expr> Parser::parseTerm() {
    std::unique_ptr<Expr> expr = parseFactor();

    while (match(TokenType::Plus) || match(TokenType::Minus)) {
        std::string op = previous().value;
        std::unique_ptr<Expr> right = parseFactor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::parseFactor() {
    std::unique_ptr<Expr> expr = parsePrimary();

    while (match(TokenType::Slash) || match(TokenType::Star)) {
        std::string op = previous().value;
        std::unique_ptr<Expr> right = parsePrimary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::vector<std::unique_ptr<Stmt>> Parser::parseBlock() {
    std::vector<std::unique_ptr<Stmt>> parsedStatements;

    while (!isAtEnd() && peek().type != TokenType::CloseBrace) {
        while (match(TokenType::Newline)) {}
        if (peek().type == TokenType::CloseBrace) break;
        parsedStatements.push_back(parseStatement());
    }

    match(TokenType::CloseBrace);
    return parsedStatements;
}

Token Parser::peek() { return tokens[current]; }
Token Parser::advance() { return tokens[current++]; }
Token Parser::previous() { return tokens[current - 1]; }

bool Parser::isAtEnd() {
    return tokens[current].type == TokenType::EndOfFile; 
}

bool Parser::match(TokenType expected) {
    if (isAtEnd() || tokens[current].type != expected) return false;
    advance();
    return true;
}