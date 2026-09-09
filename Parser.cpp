#include "Parser.h"
#include "AST.h"
#include "Token.h"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

Token Parser::peekNext() {
    if (current + 1 >= tokens.size()) return tokens.back();
    return tokens[current + 1];
}

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
    if (match(TokenType::If)) {
        return parseIfStatement();
    }
    if (match(TokenType::Repeat)) {
        return parseRepeatStatement();
    }
    if (peek().type == TokenType::Identifier && peekNext().type == TokenType::Equals) {
        return parseAssignStatement();
    }

    std::unique_ptr<Expr> expr = parseExpression();
    match(TokenType::Newline);
    return std::make_unique<ExprStmt>(std::move(expr));
}

std::unique_ptr<Stmt> Parser::parseAssignStatement() {
    Token nameToken = advance();      
    advance();                        
    std::unique_ptr<Expr> value = parseExpression();
    match(TokenType::Newline);
    return std::make_unique<AssignStmt>(nameToken.value, std::move(value));
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
    return parseEquality(); // Or parseComparison depending on your chain
}

// 2. Add comparison parsing
std::unique_ptr<Expr> Parser::parseComparison() {
    std::unique_ptr<Expr> expr = parseTerm(); // Do math first (+, -)

    while (match(TokenType::Less) || match(TokenType::Greater) || 
           match(TokenType::LessEqual) || match(TokenType::GreaterEqual)) {
        std::string op = previous().value;
        std::unique_ptr<Expr> right = parseTerm();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// 3. Add equality parsing
std::unique_ptr<Expr> Parser::parseEquality() {
    std::unique_ptr<Expr> expr = parseComparison(); // Do comparisons first (<, >)

    while (match(TokenType::EqualEqual) || match(TokenType::BangEqual)) {
        std::string op = previous().value;
        std::unique_ptr<Expr> right = parseComparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
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

    throw std::runtime_error("Unexpected token '" + peek().value + "' in expression.");
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

std::unique_ptr<Expr> Parser::parseCall() {
    std::unique_ptr<Expr> expr = parsePrimary();

    while (match(TokenType::OpenParen)) {
        std::vector<std::unique_ptr<Expr>> arguments;

        if (peek().type != TokenType::CloseParen) {
            do {
                arguments.push_back(parseExpression());
            } while (match(TokenType::Comma));
        }

        if (!match(TokenType::CloseParen)) {
            throw std::runtime_error("Expected ')' after arguments.");
        }

        std::string calleeName = "";
        if (auto varExpr = dynamic_cast<VariableExpr*>(expr.get())) {
            calleeName = varExpr->name;
        } else {
            throw std::runtime_error("Can only call named functions.");
        }

        expr = std::make_unique<CallExpr>(calleeName, std::move(arguments));
    }

    return expr;
}


std::unique_ptr<Stmt> Parser::parseIfStatement(){
    match(TokenType::OpenParen);
    std::unique_ptr<Expr> condition = parseExpression();
    match(TokenType::CloseParen);
    match(TokenType::OpenBrace);

    std::vector<std::unique_ptr<Stmt>> body = parseBlock();

    return std::make_unique<IfStmt>(std::move(condition), std::move(body));
};

std::unique_ptr<Stmt> Parser::parseRepeatStatement(){
    match(TokenType::OpenParen);
    std::unique_ptr<Expr> count = parseExpression();
    match(TokenType::CloseParen);
    match(TokenType::OpenBrace);

    std::vector<std::unique_ptr<Stmt>> body = parseBlock();
    return std::make_unique<RepeatStmt>(std::move(count), std::move(body));
}



std::unique_ptr<Expr> Parser::parseFactor() {
    std::unique_ptr<Expr> expr = parseCall();

    while (match(TokenType::Slash) || match(TokenType::Star)) {
        std::string op = previous().value;
        std::unique_ptr<Expr> right = parseCall();
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