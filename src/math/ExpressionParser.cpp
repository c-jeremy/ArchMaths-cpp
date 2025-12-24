#include "math/ExpressionParser.h"
#include <stdexcept>
#include <iostream>

namespace ArchMaths {

ExpressionParser::ExpressionParser() {}

bool ExpressionParser::isUserFunctionDefinition(const std::string& expression) const {
    // 简单检测: 查找 name(params) = body 模式
    size_t parenOpen = expression.find('(');
    if (parenOpen == std::string::npos || parenOpen == 0) return false;
    size_t parenClose = expression.find(')', parenOpen);
    if (parenClose == std::string::npos) return false;
    size_t eqPos = expression.find('=', parenClose);
    return eqPos != std::string::npos;
}

ExprNodePtr ExpressionParser::parse(const std::string& expression) {
    hasError_ = false;
    errorMessage_.clear();
    currentIndex_ = 0;

    tokens_ = tokenizer_.tokenize(expression);

    try {
        auto result = parseExpression();
        if (currentToken().type != TokenType::End) {
            hasError_ = true;
            errorMessage_ = "意外的标记: " + currentToken().value;
            return nullptr;
        }
        return result;
    } catch (const std::exception& e) {
        hasError_ = true;
        errorMessage_ = e.what();
        return nullptr;
    }
}

Token ExpressionParser::currentToken() const {
    if (currentIndex_ < tokens_.size()) {
        return tokens_[currentIndex_];
    }
    return Token(TokenType::End, "");
}

Token ExpressionParser::nextToken() {
    if (currentIndex_ < tokens_.size()) {
        return tokens_[currentIndex_++];
    }
    return Token(TokenType::End, "");
}

bool ExpressionParser::match(TokenType type) {
    if (currentToken().type == type) {
        nextToken();
        return true;
    }
    return false;
}

bool ExpressionParser::expect(TokenType type, const std::string& errorMsg) {
    if (!match(type)) {
        throw std::runtime_error(errorMsg);
    }
    return true;
}

ExprNodePtr ExpressionParser::parseExpression() {
    return parseComparison();
}

ExprNodePtr ExpressionParser::parseComparison() {
    auto left = parseAddSub();

    while (true) {
        Token token = currentToken();
        if (token.type == TokenType::Equals ||
            token.type == TokenType::LessThan ||
            token.type == TokenType::GreaterThan ||
            token.type == TokenType::LessEqual ||
            token.type == TokenType::GreaterEqual) {
            nextToken();
            auto right = parseAddSub();
            // 对于隐函数，转换为 left - right 形式
            left = ExprNode::makeBinaryOp("-", left, right);
        } else {
            break;
        }
    }

    return left;
}

ExprNodePtr ExpressionParser::parseAddSub() {
    auto left = parseMulDiv();

    while (true) {
        Token token = currentToken();
        if (token.type == TokenType::Operator &&
            (token.value == "+" || token.value == "-")) {
            nextToken();
            auto right = parseMulDiv();
            left = ExprNode::makeBinaryOp(token.value, left, right);
        } else {
            break;
        }
    }

    return left;
}

ExprNodePtr ExpressionParser::parseMulDiv() {
    auto left = parsePower();
    if (!left) return nullptr;

    while (true) {
        Token token = currentToken();
        if (token.type == TokenType::Operator &&
            (token.value == "*" || token.value == "/")) {
            nextToken();
            auto right = parsePower();
            if (!right) return nullptr;
            left = ExprNode::makeBinaryOp(token.value, left, right);
        }
        // 隐式乘法: 2x, x(y+1), (x+1)(y+1)
        else if (token.type == TokenType::Number ||
                 token.type == TokenType::Variable ||
                 token.type == TokenType::Function ||
                 token.type == TokenType::LeftParen) {
            auto right = parsePower();
            if (!right) return nullptr;
            left = ExprNode::makeBinaryOp("*", left, right);
        } else {
            break;
        }
    }

    return left;
}

ExprNodePtr ExpressionParser::parsePower() {
    auto left = parseUnary();

    if (currentToken().type == TokenType::Operator &&
        currentToken().value == "^") {
        nextToken();
        auto right = parsePower(); // 右结合
        return ExprNode::makeBinaryOp("^", left, right);
    }

    return left;
}

ExprNodePtr ExpressionParser::parseUnary() {
    Token token = currentToken();

    if (token.type == TokenType::Operator && token.value == "-") {
        nextToken();
        auto operand = parseUnary();
        return ExprNode::makeUnaryOp("-", operand);
    }

    if (token.type == TokenType::Operator && token.value == "+") {
        nextToken();
        return parseUnary();
    }

    return parsePrimary();
}

ExprNodePtr ExpressionParser::parsePrimary() {
    Token token = currentToken();

    // 数字
    if (token.type == TokenType::Number) {
        nextToken();
        return ExprNode::makeNumber(token.numValue);
    }

    // 函数调用
    if (token.type == TokenType::Function) {
        nextToken();
        return parseFunction(token.value);
    }

    // 变量 - 检查是否是用户自定义函数调用
    if (token.type == TokenType::Variable) {
        std::string varName = token.value;
        nextToken();

        // 检查是否后面跟着左括号且是用户自定义函数
        if (currentToken().type == TokenType::LeftParen && userFunctions_ && userFunctions_->count(varName) > 0) {
            std::cerr << "Found user function call: " << varName << std::endl;
            return parseFunction(varName);
        }

        return ExprNode::makeVariable(varName);
    }

    // 括号表达式
    if (token.type == TokenType::LeftParen) {
        nextToken();
        auto expr = parseExpression();
        expect(TokenType::RightParen, "缺少右括号");
        return expr;
    }

    throw std::runtime_error("意外的标记: " + token.value);
}

ExprNodePtr ExpressionParser::parseFunction(const std::string& name) {
    std::cerr << "parseFunction called for: " << name << std::endl;
    expect(TokenType::LeftParen, "函数调用缺少左括号");

    std::vector<ExprNodePtr> args;

    if (currentToken().type != TokenType::RightParen) {
        args.push_back(parseExpression());

        while (currentToken().type == TokenType::Comma) {
            nextToken();
            args.push_back(parseExpression());
        }
    }

    expect(TokenType::RightParen, "函数调用缺少右括号");
    std::cerr << "parseFunction: parsed " << args.size() << " args" << std::endl;

    // 检查是否是用户自定义函数
    if (userFunctions_) {
        auto it = userFunctions_->find(name);
        if (it != userFunctions_->end()) {
            std::cerr << "Calling substituteUserFunction for: " << name << std::endl;
            auto result = substituteUserFunction(name, args);
            std::cerr << "substituteUserFunction returned, result=" << (result ? "valid" : "null") << std::endl;
            return result;
        }
    }

    return ExprNode::makeFunction(name, std::move(args));
}

ExprNodePtr ExpressionParser::substituteUserFunction(const std::string& name, const std::vector<ExprNodePtr>& args) {
    std::cerr << "substituteUserFunction: name=" << name << ", args.size()=" << args.size() << std::endl;

    if (!userFunctions_ || userFunctions_->empty()) {
        std::cerr << "substituteUserFunction: no user functions" << std::endl;
        return ExprNode::makeFunction(name, std::vector<ExprNodePtr>(args));
    }

    auto it = userFunctions_->find(name);
    if (it == userFunctions_->end()) {
        std::cerr << "substituteUserFunction: function not found" << std::endl;
        return ExprNode::makeFunction(name, std::vector<ExprNodePtr>(args));
    }

    const UserFunction& func = it->second;
    std::cerr << "substituteUserFunction: bodyStr=" << func.bodyStr << ", params.size()=" << func.params.size() << std::endl;

    if (func.bodyStr.empty()) {
        std::cerr << "substituteUserFunction: empty body" << std::endl;
        return ExprNode::makeFunction(name, std::vector<ExprNodePtr>(args));
    }
    if (args.size() != func.params.size()) {
        throw std::runtime_error("函数 " + name + " 参数数量不匹配: 期望 " +
            std::to_string(func.params.size()) + " 个参数，实际 " + std::to_string(args.size()) + " 个");
    }

    std::cerr << "substituteUserFunction: creating bodyParser" << std::endl;
    // 使用新的解析器实例解析函数体（避免状态污染）
    ExpressionParser bodyParser;
    std::cerr << "substituteUserFunction: parsing body" << std::endl;
    ExprNodePtr bodyExpr = bodyParser.parse(func.bodyStr);
    std::cerr << "substituteUserFunction: body parsed, bodyExpr=" << (bodyExpr ? "valid" : "null") << std::endl;
    if (!bodyExpr || bodyParser.hasError()) {
        throw std::runtime_error("函数体解析失败: " + bodyParser.getError());
    }

    std::cerr << "substituteUserFunction: building subs map" << std::endl;
    // 构建参数替换映射
    std::unordered_map<std::string, ExprNodePtr> subs;
    for (size_t i = 0; i < func.params.size(); ++i) {
        std::cerr << "substituteUserFunction: param[" << i << "]=" << func.params[i] << std::endl;
        if (!args[i]) {
            throw std::runtime_error("函数参数为空");
        }
        subs[func.params[i]] = args[i];
    }

    std::cerr << "substituteUserFunction: calling cloneAndSubstitute" << std::endl;
    // 克隆函数体并替换参数
    auto result = cloneAndSubstitute(bodyExpr, subs);
    std::cerr << "substituteUserFunction: cloneAndSubstitute done, result=" << (result ? "valid" : "null") << std::endl;
    if (!result) {
        throw std::runtime_error("函数替换失败");
    }
    return result;
}

ExprNodePtr ExpressionParser::cloneAndSubstitute(const ExprNodePtr& node, const std::unordered_map<std::string, ExprNodePtr>& subs) {
    if (!node) return nullptr;

    switch (node->type) {
        case NodeType::Number:
            return ExprNode::makeNumber(node->value);

        case NodeType::Variable: {
            auto it = subs.find(node->name);
            if (it != subs.end() && it->second) {
                // 深拷贝替换的表达式 - 使用空map避免无限递归
                return cloneAndSubstitute(it->second, {});
            }
            return ExprNode::makeVariable(node->name);
        }

        case NodeType::BinaryOp: {
            auto left = cloneAndSubstitute(node->left, subs);
            auto right = cloneAndSubstitute(node->right, subs);
            if (!left || !right) return nullptr;
            return ExprNode::makeBinaryOp(node->op, left, right);
        }

        case NodeType::UnaryOp: {
            auto operand = cloneAndSubstitute(node->left, subs);
            if (!operand) return nullptr;
            return ExprNode::makeUnaryOp(node->op, operand);
        }

        case NodeType::Function: {
            std::vector<ExprNodePtr> newArgs;
            for (const auto& arg : node->args) {
                auto newArg = cloneAndSubstitute(arg, subs);
                if (!newArg) return nullptr;
                newArgs.push_back(newArg);
            }
            return ExprNode::makeFunction(node->name, std::move(newArgs));
        }

        default:
            return nullptr;
    }
}

} // namespace ArchMaths
