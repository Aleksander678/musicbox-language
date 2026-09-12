#include "Lexer.h"
#include <cctype>
#include<utility>

Lexer::Lexer(std::string source) : source(std::move(source)) {}

std::vector<Token> Lexer::tokenize() {
    while (!isAtEnd()) {
        start = current;
        startLine = line;
        startCol = col;
        scanToken();
    }
    tokens.emplace_back("EOF", TokenType::EndOfFile, line, col);
    return tokens;
}

const std::vector<std::string>& Lexer::getErrors() const { 
    return errors; 
}

const std::unordered_map<std::string, TokenType>& Lexer::keywords() {
    static const std::unordered_map<std::string, TokenType> map = {
        {"declare", TokenType::Declare},
        {"function", TokenType::Function},
        {"return", TokenType::Return},
        {"repeat", TokenType::Repeat},
        {"condition", TokenType::Condition},
    };
    return map;
}

bool Lexer::isAtEnd() const { return current >= source.size(); }

char Lexer::peek() const { return isAtEnd() ? '\0' : source[current]; }
char Lexer::peekNext() const {
    return current + 1 >= source.size() ? '\0' : source[current + 1];
}

char Lexer::advance() {
    char c = source[current++];
    if (c == '\n') { line++; col = 1; }
    else { col++; }
    return c;
}

bool Lexer::match(char expected) {
    if (isAtEnd() || source[current] != expected) return false;
    advance();
    return true;
}

void Lexer::addToken(TokenType type) {
    addToken(type, source.substr(start, current - start));
}

void Lexer::addToken(TokenType type, std::string value) {
    tokens.emplace_back(std::move(value), type, startLine, startCol);
}

void Lexer::error(const std::string& message) {
    errors.push_back("[line " + std::to_string(startLine) + ", col " +
                     std::to_string(startCol) + "] " + message);
}

bool Lexer::isDigit(char c) { return c >= '0' && c <= '9'; }
bool Lexer::isAlpha(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}
bool Lexer::isAlphaNumeric(char c) { return isAlpha(c) || isDigit(c); }

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::OpenParen); break;
        case ')': addToken(TokenType::CloseParen); break;
        case '+': addToken(TokenType::Plus); break;
        case '-': addToken(TokenType::Minus); break;
        case '*': addToken(TokenType::Star); break;
        case ',': addToken(TokenType::Comma); break;
        case '{': addToken(TokenType::OpenBrace); break;
        case '}': addToken(TokenType::CloseBrace); break;

        case '=':
            addToken(match('=') ? TokenType::EqualEqual : TokenType::Equals);
            break;
        case '<':
            addToken(match('=') ? TokenType::LessEqual : TokenType::Less);
            break;
        case '>':
            addToken(match('=') ? TokenType::GreaterEqual : TokenType::Greater);
            break;

        case '/':
            if (match('/')) {
                while (peek() != '\n' && !isAtEnd()) advance();
            } else {
                addToken(TokenType::Slash);
            }
            break;
        
        case '\n':
            addToken(TokenType::Newline);
            break;
        case ' ':
        case '\r':
        case '\t':
            break;

        case '"': string_(); break;

        default:
            if (isDigit(c)) {
                number();
            } else if (isAlpha(c)) {
                identifier();
            } else {
                error(std::string("Unexpected character '") + c + "'");
            }
            break;
    }
}

void Lexer::string_() {
    while (peek() != '"' && !isAtEnd()) advance();

    if (isAtEnd()) {
        error("Unterminated string literal");
        return;
    }
    advance();

    std::string value = source.substr(start + 1, (current - 1) - (start + 1));
    addToken(TokenType::String, value);
}

void Lexer::number() {
    while (isDigit(peek())) advance();

    if (peek() == '.' && isDigit(peekNext())) {
        advance(); 
        while (isDigit(peek())) advance();
    }

    addToken(TokenType::Number);
}

void Lexer::identifier() {
    while (isAlphaNumeric(peek())) advance();

    std::string text = source.substr(start, current - start);
    const auto& kw = keywords();
    auto it = kw.find(text);
    addToken(it != kw.end() ? it->second : TokenType::Identifier, text);
}