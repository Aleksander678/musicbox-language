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
#include "AstPrinter.h"


std::string AstPrinter::print(const std::vector<std::unique_ptr<Stmt>>& statements) {
    std::string result = "";
    for (const auto& stmt : statements) {
        result += printStmt(stmt.get()) + "\n";
    }
    return result;
}

std::string AstPrinter::printStmt(Stmt* stmt) {
    if (auto letStmt = dynamic_cast<DeclareStmt*>(stmt)) {
        return "(let " + letStmt->name + " = " + printExpr(letStmt->initializer.get()) + ")";
    }
    if (auto retStmt = dynamic_cast<ReturnStmt*>(stmt)) {
        return "(return " + printExpr(retStmt->value.get()) + ")";
    }
    if (auto assignStmt = dynamic_cast<AssignStmt*>(stmt)) {
        return "(assign " + assignStmt->name + " = " + printExpr(assignStmt->value.get()) + ")";
    }
    if (auto exprStmt = dynamic_cast<ExprStmt*>(stmt)) {
        return printExpr(exprStmt->expression.get());
    }
    if (auto funStmt = dynamic_cast<FunctionStmt*>(stmt)) {
        std::string bodyStr = "";
        for (const auto& s : funStmt->body) {
            bodyStr += "  " + printStmt(s.get()) + "\n";
        }
        return "(fun " + funStmt->name + "(...) {\n" + bodyStr + "})";
    }
    if (auto condStmt = dynamic_cast<ConditionStmt*>(stmt)) {
        std::string bodyStr = "";
        for (const auto& s : condStmt->body) {
            bodyStr += "  " + printStmt(s.get()) + "\n";
        }
        return "(if " + printExpr(condStmt->condition.get()) + " {\n" + bodyStr + "})";
    }
    if (auto repStmt = dynamic_cast<RepeatStmt*>(stmt)) {
        std::string bodyStr = "";
        for (const auto& s : repStmt->body) {
            bodyStr += "  " + printStmt(s.get()) + "\n";
        }
        return "(repeat " + printExpr(repStmt->count.get()) + " {\n" + bodyStr + "})";
    }
    return "(unknown stmt)";
}

std::string AstPrinter::printExpr(Expr* expr) {
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
    if (auto call = dynamic_cast<CallExpr*>(expr)) {
        std::string argsStr = "";
        for (size_t i = 0; i < call->arguments.size(); i++) {
            if (i > 0) argsStr += " ";
            argsStr += printExpr(call->arguments[i].get());
        }
        return "(call " + call->callee + " " + argsStr + ")";
    }
    return "(unknown expr)";
}
