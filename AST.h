#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include <utility>
// ============================================================
// EXPRESSIONS
// ============================================================

struct Expr {
    virtual ~Expr() = default;
};

// 10
// "hello"
struct LiteralExpr : Expr {
    std::string value;

    LiteralExpr(std::string value)
        : value(std::move(value)) {}
};

// x
struct VariableExpr : Expr {
    std::string name;

    VariableExpr(std::string name)
        : name(std::move(name)) {}
};

// a + b
// x * 10
struct BinaryExpr : Expr {
    std::unique_ptr<Expr> left;
    std::string op;
    std::unique_ptr<Expr> right;

    BinaryExpr(
        std::unique_ptr<Expr> left,
        std::string op,
        std::unique_ptr<Expr> right
    )
        : left(std::move(left)),
          op(std::move(op)),
          right(std::move(right)) {}
};

// ============================================================
// STATEMENTS
// ============================================================

struct Stmt {
    virtual ~Stmt() = default;
};

// let x = 10
struct LetStmt : Stmt {
    std::string name;
    std::unique_ptr<Expr> initializer;

    LetStmt(
        std::string name,
        std::unique_ptr<Expr> initializer
    )
        : name(std::move(name)),
          initializer(std::move(initializer)) {}
};

// return a + b
struct ReturnStmt : Stmt {
    std::unique_ptr<Expr> value;

    ReturnStmt(std::unique_ptr<Expr> value)
        : value(std::move(value)) {}
};

// fun add(a, b) {
//     return a + b
// }
struct FunctionStmt : Stmt {
    std::string name;
    std::vector<std::string> parameters;
    std::vector<std::unique_ptr<Stmt>> body;

    FunctionStmt(
        std::string name,
        std::vector<std::string> parameters,
        std::vector<std::unique_ptr<Stmt>> body
    )
        : name(std::move(name)),
          parameters(std::move(parameters)),
          body(std::move(body)) {}
};

#endif