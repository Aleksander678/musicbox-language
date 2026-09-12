#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <cstddef>
#include "Token.h"

class Lexer {
public:
    explicit Lexer(std::string source);
    std::vector<Token> tokenize();
    const std::vector<std::string>& getErrors() const;

private:
    static const std::unordered_map<std::string, TokenType>& keywords();
    bool isAtEnd() const;
    char peek() const;
    char peekNext() const;
    char advance();
    bool match(char expected);
    void addToken(TokenType type);
    void addToken(TokenType type, std::string value);
    void error(const std::string& message);
    
    static bool isDigit(char c);
    static bool isAlpha(char c);
    static bool isAlphaNumeric(char c);

    void scanToken();
    void string_();
    void number();
    void identifier();

    std::string source;
    std::vector<Token> tokens;
    std::vector<std::string> errors;
    std::size_t start = 0;
    std::size_t current = 0;
    int line = 1;
    int col = 1;
    int startLine = 1;
    int startCol = 1;
};

