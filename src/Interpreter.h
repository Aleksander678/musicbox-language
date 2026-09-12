#pragma once

#include "AST.h"
#include "environment.h"
#include <vector>
#include <memory>

std::string stringify(const RuntimeValue& v);

class Interpreter {
private:
    Environment environment;

public:
    void interpret(const std::vector<std::unique_ptr<Stmt>>& statements);
    std::unordered_map<std::string, FunctionStmt*> functions;   
    void execute(Stmt* stmt, Environment& env);                 
    RuntimeValue evaluate(Expr* expr, Environment& env);         
    RuntimeValue callFunction(CallExpr* callExpr, Environment& callerEnv);
};

