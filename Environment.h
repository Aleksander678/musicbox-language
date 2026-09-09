#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include<variant>
#include<string>
#include<unordered_map>

struct RuntimeValue{
    enum class Type {Number, String, Boolean, Null};
    Type type;
    std::variant<double, std::string, bool, nullptr_t> value;
};

class Environment {
private:
    std::unordered_map<std::string, RuntimeValue> values;
    Environment* enclosing = nullptr;
public:
    Environment() = default;                              
    explicit Environment(Environment* enclosing) : enclosing(enclosing) {}

    void define(const std::string& name, RuntimeValue value);
    RuntimeValue get(const std::string& name);
    void assign(const std::string& name, RuntimeValue value);
};

#endif