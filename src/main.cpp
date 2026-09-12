#include <iostream>
#include <vector>
#include <memory>
#include "Token.h"
#include "AST.h"
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include "Environment.h"
#include <filesystem>
#include<algorithm>
#include <fstream>   
#include <sstream>
#include "style.h"
#include<filesystem>

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
        if (auto letStmt = dynamic_cast<DeclareStmt*>(stmt)) {
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

void run(const std::string& code, Interpreter& interpreter) {
    try {
        Lexer lexer(code);
        std::vector<Token> tokens = lexer.tokenize();
        
        Parser parser(tokens);
        std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

        interpreter.interpret(statements);
    } catch (const std::exception& e) {
        style::error(std::string(e.what()));
    }
}

void runFile(const std::string& filename){
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'\n";
        exit(1);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    
    Interpreter interpreter;
    run(buffer.str(), interpreter);
}

void runPrompt() {
    Interpreter interpreter; 
    std::string line;
    
    style::banner();
    
    std::string accumulatedCode; 

    for (;;) {
        std::cout << ">>> ";

        if (!std::getline(std::cin, line)) {
            break; 
        }

        if (line == "exit") {
            break;
        }

        if (line.empty()) {
            if(!accumulatedCode.empty()){
                run(accumulatedCode, interpreter);
                accumulatedCode="";
            }
            continue;
        }

        accumulatedCode += line + "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        style::error("Usage: interpreter [script.mbx]");
        return 1;
    } else if (argc == 2) {
        std::filesystem::path path(argv[1]);
        std::string ext = path.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext != ".mbx") {
            style::error("Error: expected a .mbx file, got '" 
                    + path.extension().string());
            return 2;
        }

        runFile(argv[1]);
    } else {
        runPrompt();
    }

    return 0;
}

// To run: 
// g++ -std=c++17 -o musicbox main.cpp Lexer.cpp Parser.cpp Environment.cpp interpreter.cpp Token.cpp
// ./musicbox