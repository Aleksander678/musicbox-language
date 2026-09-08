#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include<utility>

enum class TokenType {
    Number,
    String,
    Identifier,
    Let,
    Equals,
    Plus,
    Minus,
    Star,
    Slash,
    OpenParen,
    CloseParen,
    EndOfFile,
    Fun,
    Return,
    Comma,
    OpenBrace,
    CloseBrace,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    Newline,
    EqualEqual,
    BangEqual,
};


struct Token {
    std::string value;
    TokenType type;
    int line;
    int col;

    Token(std::string val, TokenType t, int l = 0, int c = 0)
        : value(std::move(val)), type(t), line(l), col(c) {}
};

std::string tokenTypeToString(TokenType type);

#endif