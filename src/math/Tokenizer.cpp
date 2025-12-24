#include "math/Tokenizer.h"
#include <algorithm>
#include <cctype>

namespace ArchMaths {

const std::vector<std::string> Tokenizer::FUNCTIONS = {
    "sin", "cos", "tan", "asin", "acos", "atan", "atan2",
    "sinh", "cosh", "tanh", "asinh", "acosh", "atanh",
    "sqrt", "cbrt", "abs", "floor", "ceil", "round",
    "exp", "log", "log10", "log2", "ln",
    "pow", "min", "max", "mod",
    "sign", "frac",
    "sum", "prod", "int", "diff"
};

const std::vector<std::string> Tokenizer::CONSTANTS = {
    "pi", "e", "phi", "tau"
};

Tokenizer::Tokenizer() {}

std::string Tokenizer::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool Tokenizer::isOperator(char c) const {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

bool Tokenizer::isFunction(const std::string& name) const {
    std::string lower = toLower(name);
    return std::find(FUNCTIONS.begin(), FUNCTIONS.end(), lower) != FUNCTIONS.end();
}

std::vector<Token> Tokenizer::tokenize(const std::string& expression) {
    std::vector<Token> tokens;
    std::string expr = expression;

    // 移除空格
    expr.erase(std::remove_if(expr.begin(), expr.end(), ::isspace), expr.end());

    size_t i = 0;
    while (i < expr.length()) {
        char c = expr[i];

        // 数字
        if (std::isdigit(c) || (c == '.' && i + 1 < expr.length() && std::isdigit(expr[i + 1]))) {
            std::string numStr;
            bool hasDecimal = false;

            while (i < expr.length() && (std::isdigit(expr[i]) || expr[i] == '.')) {
                if (expr[i] == '.') {
                    if (hasDecimal) break;
                    hasDecimal = true;
                }
                numStr += expr[i++];
            }

            // 科学计数法
            if (i < expr.length() && (expr[i] == 'e' || expr[i] == 'E')) {
                numStr += expr[i++];
                if (i < expr.length() && (expr[i] == '+' || expr[i] == '-')) {
                    numStr += expr[i++];
                }
                while (i < expr.length() && std::isdigit(expr[i])) {
                    numStr += expr[i++];
                }
            }

            Token token(TokenType::Number, numStr);
            token.numValue = std::stod(numStr);
            tokens.push_back(token);
        }
        // 标识符（变量或函数）
        else if (std::isalpha(c) || c == '_') {
            std::string identifier;
            while (i < expr.length() && (std::isalnum(expr[i]) || expr[i] == '_')) {
                identifier += expr[i++];
            }

            std::string lower = toLower(identifier);

            // 检查是否是常量
            if (std::find(CONSTANTS.begin(), CONSTANTS.end(), lower) != CONSTANTS.end()) {
                Token token(TokenType::Number, identifier);
                if (lower == "pi") token.numValue = Constants::PI;
                else if (lower == "e") token.numValue = Constants::E;
                else if (lower == "phi") token.numValue = Constants::PHI;
                else if (lower == "tau") token.numValue = Constants::TAU;
                tokens.push_back(token);
            }
            // 检查是否是函数
            else if (isFunction(identifier)) {
                tokens.push_back(Token(TokenType::Function, lower));
            }
            // 变量 - 支持隐式乘法 (xy -> x*y)
            else {
                for (size_t k = 0; k < lower.length(); ++k) {
                    if (k > 0) {
                        tokens.push_back(Token(TokenType::Operator, "*"));
                    }
                    tokens.push_back(Token(TokenType::Variable, std::string(1, lower[k])));
                }
            }
        }
        // 运算符
        else if (isOperator(c)) {
            tokens.push_back(Token(TokenType::Operator, std::string(1, c)));
            i++;
        }
        // 括号
        else if (c == '(') {
            tokens.push_back(Token(TokenType::LeftParen, "("));
            i++;
        }
        else if (c == ')') {
            tokens.push_back(Token(TokenType::RightParen, ")"));
            i++;
        }
        // 逗号
        else if (c == ',') {
            tokens.push_back(Token(TokenType::Comma, ","));
            i++;
        }
        // 比较运算符
        else if (c == '=') {
            tokens.push_back(Token(TokenType::Equals, "="));
            i++;
        }
        else if (c == '<') {
            if (i + 1 < expr.length() && expr[i + 1] == '=') {
                tokens.push_back(Token(TokenType::LessEqual, "<="));
                i += 2;
            } else {
                tokens.push_back(Token(TokenType::LessThan, "<"));
                i++;
            }
        }
        else if (c == '>') {
            if (i + 1 < expr.length() && expr[i + 1] == '=') {
                tokens.push_back(Token(TokenType::GreaterEqual, ">="));
                i += 2;
            } else {
                tokens.push_back(Token(TokenType::GreaterThan, ">"));
                i++;
            }
        }
        else {
            // 跳过未知字符
            i++;
        }
    }

    tokens.push_back(Token(TokenType::End, ""));
    return tokens;
}

} // namespace ArchMaths
