#include <iostream>
#include <vector>
#include <memory>
#include "Token.h"
#include "AST.h"
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include "Environment.h"

class AstPrinter {
public:
    std::string print(const std::vector<std::unique_ptr<Stmt>>& statements) {
        std::string result = "";
        for (const auto& stmt : statements) {
            result += printStmt(stmt.get()) + "\n";
        }
        return result;
    }

private:
    std::string printStmt(Stmt* stmt) {
        if (auto letStmt = dynamic_cast<LetStmt*>(stmt)) {
            return "(let " + letStmt->name + " = " + printExpr(letStmt->initializer.get()) + ")";
        }
        if (auto retStmt = dynamic_cast<ReturnStmt*>(stmt)) {
            return "(return " + printExpr(retStmt->value.get()) + ")";
        }
        if (auto funStmt = dynamic_cast<FunctionStmt*>(stmt)) {
            std::string bodyStr = "";
            for (const auto& s : funStmt->body) {
                bodyStr += "  " + printStmt(s.get()) + "\n";
            }
            return "(fun " + funStmt->name + "(...) {\n" + bodyStr + "})";
        }
        return "(unknown stmt)";
    }

    std::string printExpr(Expr* expr) {
        if (!expr) return "nil";

        if (auto lit = dynamic_cast<LiteralExpr*>(expr)) {
            return lit->value;
        }
        if (auto var = dynamic_cast<VariableExpr*>(expr)) {
            return var->name;
        }
        if (auto bin = dynamic_cast<BinaryExpr*>(expr)) {
            return "(" + bin->op + " " + printExpr(bin->left.get()) + " " + printExpr(bin->right.get()) + ")";
        }
        return "(unknown expr)";
    }
};

int main() {
    std::string code =
        "let x = 10 + 4 * 2.1\n"
        "\n"
        "fun add(a, b){\n"
        "\n"
        "return a + b\n"
        "\n"
        "}\n"
        "let y = add(x, 3) + 2 * 3\n"
        "print(y)\n";

    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    // AstPrinter printer;
    // std::cout << printer.print(statements) << std::endl;

    Interpreter interpreter;
    interpreter.interpret(statements);

    return 0;
}

// g++ -std=c++17 -o interpreter main.cpp Lexer.cpp Parser.cpp Environment.cpp interpreter.cpp Token.cpp
// ./interpreter