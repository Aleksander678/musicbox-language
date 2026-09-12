#include "Token.h"

std::string tokenTypeToString(TokenType type){
    switch (type) {
        case TokenType::Number:       return "Number";
        case TokenType::String:       return "String";
        case TokenType::Identifier:   return "Identifier";
        case TokenType::Declare:          return "Declare";
        case TokenType::Equals:       return "Equals";
        case TokenType::Plus:         return "Plus";
        case TokenType::Minus:        return "Minus";
        case TokenType::Star:         return "Star";
        case TokenType::Slash:        return "Slash";
        case TokenType::OpenParen:    return "OpenParen";
        case TokenType::CloseParen:   return "CloseParen";
        case TokenType::Function:           return "Function";
        case TokenType::Return:       return "Return";
        case TokenType::Comma:        return "Comma";
        case TokenType::OpenBrace:    return "OpenBrace";
        case TokenType::CloseBrace:   return "CloseBrace";
        case TokenType::Less:         return "Less";
        case TokenType::LessEqual:    return "LessEqual";
        case TokenType::Greater:      return "Greater";
        case TokenType::GreaterEqual: return "GreaterEqual";
        case TokenType::EqualEqual:   return "EqualEqual";
        case TokenType::EndOfFile:    return "EndOfFile";
        case TokenType::Newline:    return "Newline";
        case TokenType::Condition:         return "Condition";
        case TokenType::Repeat:      return "repeat";
    }
    return "Unknown";
}