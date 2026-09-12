#include "interpreter.h"
#include <iostream>
#include "style.h"

struct ReturnException{
    RuntimeValue value;
};


bool isTruthy(const RuntimeValue& v) {
    switch (v.type) {
        case RuntimeValue::Type::Boolean: return std::get<bool>(v.value);
        case RuntimeValue::Type::Number:  return std::get<double>(v.value) != 0;
        case RuntimeValue::Type::Null:    return false;
        default:                          return true;
    }
}

void Interpreter::interpret(const std::vector<std::unique_ptr<Stmt>>& statements){
    // fill in the function list
    for (const auto& stmt : statements) {
        if (auto funcStmt = dynamic_cast<FunctionStmt*>(stmt.get())) {
            functions[funcStmt->name] = funcStmt;
        }
    }

    for (const auto& stmt : statements ){
        execute(stmt.get(), environment);   
    }
}
    
void Interpreter::execute(Stmt* stmt, Environment& env){
    DeclareStmt* declareStmt = dynamic_cast<DeclareStmt*>(stmt); 

    if (declareStmt != nullptr) {
        

        RuntimeValue val = evaluate(declareStmt->initializer.get(), env);

        env.define(declareStmt->name, val);
        return;
    }

    if (auto funcStmt = dynamic_cast<FunctionStmt*>(stmt)){

        functions[funcStmt->name] = funcStmt;
        return;
    }

    if (auto conditionStmt = dynamic_cast<ConditionStmt*>(stmt)){
        RuntimeValue cond = evaluate(conditionStmt->condition.get(), env);
        if(isTruthy(cond)){
            for (const auto& st : conditionStmt->body){
                execute(st.get(), env);
            }
        }

        return;
    }

    if (auto repeatStmt = dynamic_cast<RepeatStmt*>(stmt)){
        RuntimeValue count = evaluate(repeatStmt->count.get(), env);
        if(count.type != RuntimeValue::Type::Number) throw std::runtime_error("repeat(...) requires an integer argument");

        int n = static_cast<int>(std::get<double>(count.value));

        for(int i = 0; i < n; i++){
            for (const auto& st : repeatStmt->body){
                execute(st.get(), env);
            }
        }
        return;
    }

    if (auto assignStmt = dynamic_cast<AssignStmt*>(stmt)) {
        RuntimeValue val = evaluate(assignStmt->value.get(), env);
        env.assign(assignStmt->name, val);
        return;
    }

    if (auto exprStmt = dynamic_cast<ExprStmt*>(stmt)) {
        evaluate(exprStmt->expression.get(), env);   
        return;
    }

    if (auto retStmt = dynamic_cast<ReturnStmt*>(stmt)) {           
        RuntimeValue val = retStmt->value
            ? evaluate(retStmt->value.get(), env)
            : RuntimeValue{RuntimeValue::Type::Null, nullptr};
        throw ReturnException{val};
    }

}

std::string stringify(const RuntimeValue& v) {
    switch (v.type) {
        case RuntimeValue::Type::Null:   return "null";
        case RuntimeValue::Type::Number: return std::to_string(std::get<double>(v.value));
        case RuntimeValue::Type::String: return std::get<std::string>(v.value);
        case RuntimeValue::Type::Boolean:   return std::get<bool>(v.value) ? "true" : "false";
    }
    return "";
}

RuntimeValue Interpreter::callFunction(CallExpr* callExpr, Environment& callerEnv){

    // print function
    if (callExpr->callee == "display") {
        for (auto& argExpr : callExpr->arguments) {
            RuntimeValue v = evaluate(argExpr.get(), callerEnv);
            style::display(stringify(v));
        }
        std::cout << "\n";
        return {RuntimeValue::Type::Null, nullptr};
    }

    auto it = functions.find(callExpr->callee);
    if (it == functions.end()) {
        throw std::runtime_error("Undefined function '" + callExpr->callee + "'.");
    }
    FunctionStmt* func = it->second;

    std::vector<RuntimeValue> args;
    for (auto& argExpr : callExpr->arguments) {
        args.push_back(evaluate(argExpr.get(), callerEnv));  
    }

    Environment callEnv(&environment);
    for (size_t i = 0; i < func->parameters.size(); i++) {
        callEnv.define(func->parameters[i], args[i]);
    }

    try {
        for (const auto& stmt : func->body) {
            execute(stmt.get(), callEnv);  
        }
    } catch (ReturnException& ret) {
        return ret.value;
    }

    return {RuntimeValue::Type::Null, nullptr};
}


    
RuntimeValue Interpreter::evaluate(Expr* expr, Environment& env){
    LiteralExpr*  literalExpr = dynamic_cast<LiteralExpr*>(expr);

    if (literalExpr != nullptr){
        RuntimeValue val;
        try {
            val.type = RuntimeValue::Type::Number;
            val.value = std::stod(literalExpr->value);
        } catch (...) {
            val.type = RuntimeValue::Type::String;
            val.value = literalExpr->value;
        }
        return val;
    }

    BinaryExpr*  binaryExpr = dynamic_cast<BinaryExpr*>(expr);

    if (binaryExpr != nullptr){
        RuntimeValue left = evaluate(binaryExpr->left.get(), env);
        RuntimeValue right = evaluate(binaryExpr->right.get(), env);
        
        // BINARY OPERATORS
        if (binaryExpr->op == "+" || binaryExpr->op == "-" || binaryExpr->op == "*" || binaryExpr->op == "/") {
            // Numbers
            if (left.type == RuntimeValue::Type::Number && right.type == RuntimeValue::Type::Number) {
                double l = std::get<double>(left.value);
                double r = std::get<double>(right.value);
                
                RuntimeValue result;
                result.type = RuntimeValue::Type::Number;
                
                if (binaryExpr->op == "+") result.value = l + r;
                else if (binaryExpr->op == "-") result.value = l - r;
                else if (binaryExpr->op == "*") result.value = l * r;
                else if (binaryExpr->op == "/") {
                    if (r == 0) throw std::runtime_error("Division by zero.");
                    result.value = l / r;
                }
                return result;
            }
            // Strings
            if (binaryExpr->op == "+" && left.type == RuntimeValue::Type::String && right.type == RuntimeValue::Type::String) {
                RuntimeValue result;
                result.type = RuntimeValue::Type::String;
                result.value = std::get<std::string>(left.value) + std::get<std::string>(right.value);
                return result;
            }
            throw std::runtime_error("Invalid operands for arithmetic operator.");
        }

        // COMPARISON OPERATORS 
       if (binaryExpr->op == "<" || binaryExpr->op == ">" || binaryExpr->op == "<=" || binaryExpr->op == ">=") {
            RuntimeValue result;
            result.type = RuntimeValue::Type::Boolean;

            if (left.type == RuntimeValue::Type::Number && right.type == RuntimeValue::Type::Number) {
                double l = std::get<double>(left.value);
                double r = std::get<double>(right.value);
                
                if (binaryExpr->op == "<") result.value = l < r;
                else if (binaryExpr->op == ">") result.value = l > r;
                else if (binaryExpr->op == "<=") result.value = l <= r;
                else if (binaryExpr->op == ">=") result.value = l >= r;
                return result;
            }
            if (left.type == RuntimeValue::Type::String && right.type == RuntimeValue::Type::String) {
                std::string l = std::get<std::string>(left.value);
                std::string r = std::get<std::string>(right.value);
                
                if (binaryExpr->op == "<") result.value = l < r;
                else if (binaryExpr->op == ">") result.value = l > r;
                else if (binaryExpr->op == "<=") result.value = l <= r;
                else if (binaryExpr->op == ">=") result.value = l >= r;
                return result;
            }
            throw std::runtime_error("Comparison operators require matching types.");
        }

        // (IN)EQUALITY OPERATORS
        if (binaryExpr->op == "==" || binaryExpr->op == "!=") {
            RuntimeValue result;
            result.type = RuntimeValue::Type::Boolean;

            bool isEqual = false;
            if (left.type == right.type) {
                if (left.type == RuntimeValue::Type::Number) {
                    isEqual = std::get<double>(left.value) == std::get<double>(right.value);
                } else if (left.type == RuntimeValue::Type::String) {
                    isEqual = std::get<std::string>(left.value) == std::get<std::string>(right.value);
                } else if (left.type == RuntimeValue::Type::Boolean) {
                    isEqual = std::get<bool>(left.value) == std::get<bool>(right.value);
                } else if (left.type == RuntimeValue::Type::Null) {
                    isEqual = true;
                }
            }

            if (binaryExpr->op == "==") result.value = isEqual;
            else result.value = !isEqual;

            return result;
        }

        return {RuntimeValue::Type::Null, nullptr};
    }

    if (auto callExpr = dynamic_cast<CallExpr*>(expr)) {             
        return callFunction(callExpr, env);
    }

    VariableExpr*  variableExpr = dynamic_cast<VariableExpr*>(expr);

    if (variableExpr != nullptr){
        return env.get(variableExpr->name);
    }

    return {RuntimeValue::Type::Null, nullptr};
}