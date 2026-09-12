#pragma once

#include<string>
#include<iostream>

namespace style {
    constexpr const char* RESET  = "\033[0m";
    constexpr const char* RED    = "\033[31m";
    constexpr const char* GREEN  = "\033[32m";
    constexpr const char* YELLOW = "\033[33m";
    constexpr const char* CYAN   = "\033[36m";

    inline void error(const std::string& errorMessage){
        std::cerr<< RED << "Error: " << RESET << errorMessage << "\n"; 
    }

    inline void display(const std::string& message) {
        std::cout << GREEN << message << RESET << "\n";
    }

    inline void banner() {
        std::cout << CYAN 
                << "+--------------------------------------+\n"
                << "|         < < < MUSIC BOX > > >        |\n"
                << "+--------------------------------------+\n"
                << GREEN 
                << " Type 'exit' to quit | Enter twice to run\n" 
                << RESET << "\n" ;
    }
    
}

