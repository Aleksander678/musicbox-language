#pragma once
#include <string>
#include <vector>
#include <memory>
#include "AST.h"

class AstPrinter {
public:
    std::string print(const std::vector<std::unique_ptr<Stmt>>& statements);

private:
    std::string printStmt(Stmt* stmt);
    std::string printExpr(Expr* expr);
};