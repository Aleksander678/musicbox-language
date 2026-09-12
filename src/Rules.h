#pragma once
#include <string>
#include<algorithm>

inline bool isNameValid(std::string name){
    static const std::vector<std::string> syllables = {"do", "re", "mi", "fa", "sol", "la", "ti"};
    
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);

    for (const std::string syllable : syllables) {
        if(name.find(syllable) != std::string::npos) return true;
    };

    return false;
}   