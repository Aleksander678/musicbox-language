#pragma once

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

// add(5, 3)
struct CallExpr : Expr {
    std::string callee; 
    std::vector<std::unique_ptr<Expr>> arguments; 

    CallExpr(std::string callee, std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(callee)), arguments(std::move(arguments)) {}
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

struct ExprStmt : Stmt {
    std::unique_ptr<Expr> expression;

    ExprStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}
};


// declare x = 10
struct DeclareStmt : Stmt {
    std::string name;
    std::unique_ptr<Expr> initializer;

    DeclareStmt(
        std::string name,
        std::unique_ptr<Expr> initializer
    )
        : name(std::move(name)),
          initializer(std::move(initializer)) {}
};

struct ConditionStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::vector<std::unique_ptr<Stmt>> body;

    ConditionStmt(
        std::unique_ptr<Expr> condition,
        std::vector<std::unique_ptr<Stmt>> body
    )
        : condition(std::move(condition)),
          body(std::move(body)) {}
};


struct RepeatStmt : Stmt {
    std::unique_ptr<Expr> count;
    std::vector<std::unique_ptr<Stmt>> body;

    RepeatStmt(
        std::unique_ptr<Expr> count,
        std::vector<std::unique_ptr<Stmt>> body
    )
        : count(std::move(count)),
          body(std::move(body)) {}
};

struct AssignStmt : Stmt {
    std::string name;
    std::unique_ptr<Expr> value;

    AssignStmt(std::string name, std::unique_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {}
};

// return a + b
struct ReturnStmt : Stmt {
    std::unique_ptr<Expr> value;

    ReturnStmt(std::unique_ptr<Expr> value)
        : value(std::move(value)) {}
};

// function add(a, b) {
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

