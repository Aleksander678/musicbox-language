#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include "Token.h"
#include "AST.h"

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<Stmt>> parse();

private:
    std::unique_ptr<Stmt> parseStatement();
    std::unique_ptr<Stmt> parseLetStatement();
    std::unique_ptr<Stmt> parseFunctionStatement();
    std::unique_ptr<Stmt> parseReturnStatement();
    std::unique_ptr<Expr> parseExpression();
    std::unique_ptr<Expr> parsePrimary();
    std::unique_ptr<Expr> parseTerm();
    std::unique_ptr<Expr> parseFactor();
    std::unique_ptr<Expr> parseEquality();
    std::unique_ptr<Expr> parseComparison();
    std::vector<std::unique_ptr<Stmt>> parseBlock();
    std::unique_ptr<Expr> parseCall();

    std::unique_ptr<Stmt> parseIfStatement();
    std::unique_ptr<Stmt> parseRepeatStatement();
    std::unique_ptr<Stmt> parseAssignStatement();
    Token peekNext();
    Token peek();
    Token advance();
    Token previous();
    bool isAtEnd();
    bool match(TokenType expected);

    const std::vector<Token>& tokens;
    size_t current = 0;
};

#endif