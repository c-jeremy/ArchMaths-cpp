#pragma once

#include "math/MathTypes.h"
#include <string>
#include <vector>

namespace ArchMaths {

class Tokenizer {
public:
    Tokenizer();

    std::vector<Token> tokenize(const std::string& expression);

private:
    bool isOperator(char c) const;
    bool isFunction(const std::string& name) const;
    std::string toLower(const std::string& str) const;

    static const std::vector<std::string> FUNCTIONS;
    static const std::vector<std::string> CONSTANTS;
};

} // namespace ArchMaths
