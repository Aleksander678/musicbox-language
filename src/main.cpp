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
#include "ASTPrinter.h"


void run(const std::string& code, Interpreter& interpreter, bool showTree) {
    try {
        Lexer lexer(code);
        std::vector<Token> tokens = lexer.tokenize();
        
        Parser parser(tokens);
        std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

        if (showTree) {
            AstPrinter printer;
            std::cout << printer.print(statements);
            return;
        }

        interpreter.interpret(statements);
    } catch (const std::exception& e) {
        style::error(std::string(e.what()));
    }
}

void runFile(const std::string& filename, bool showTree){
    std::ifstream file(filename);
    if (!file.is_open()) {
        style::error("Error: Could not open file '" +  filename + "'\n");
        exit(1);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    
    Interpreter interpreter;
    run(buffer.str(), interpreter, showTree);
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
                run(accumulatedCode, interpreter, false);
                accumulatedCode="";
            }
            continue;
        }

        accumulatedCode += line + "\n";
    }
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);
    bool showTree = false;
    std::string filename;


    for (const auto& arg : args){
        if (arg == "--tree"){ // --tree prints the ast tree
            showTree = true;
        } else if (filename.empty()){
            filename = arg;
        } else {
            style::error("Usage: interpreter [--tree] [script.mbx]");
            return 1;
        }
    }

    if (filename.empty()) {
        if (showTree) {
            style::error("--tree requires a script file.");
            return 1;
        }
        runPrompt();
        return 0;
    }

    std::filesystem::path path(filename);
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext != ".mbx") {
        style::error("Expected a .mbx file, got '" + path.extension().string() + "'");
        return 2;
    }

    runFile(filename, showTree);
    return 0;
}

