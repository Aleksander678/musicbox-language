#include "Environment.h"
#include <unordered_map>
#include <stdexcept>

void Environment::define(const std::string& name, RuntimeValue value){
    values[name] = value;

}

RuntimeValue Environment::get(const std::string& name){
    auto itr = values.find(name);
    if (itr != values.end()){
        return itr->second;
    }

    if (enclosing != nullptr) {
        return enclosing->get(name);
    }

    throw std::runtime_error("Undefined variable " + name + ".");
}