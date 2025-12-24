#include "math/ExpressionEvaluator.h"
#include <cmath>
#include <stdexcept>
#include <algorithm>

namespace ArchMaths {

ExpressionEvaluator::ExpressionEvaluator() {
    initBuiltinFunctions();
}

void ExpressionEvaluator::initBuiltinFunctions() {
    // 三角函数
    functions_["sin"] = [](const std::vector<double>& args) { return std::sin(args[0]); };
    functions_["cos"] = [](const std::vector<double>& args) { return std::cos(args[0]); };
    functions_["tan"] = [](const std::vector<double>& args) { return std::tan(args[0]); };
    functions_["asin"] = [](const std::vector<double>& args) { return std::asin(args[0]); };
    functions_["acos"] = [](const std::vector<double>& args) { return std::acos(args[0]); };
    functions_["atan"] = [](const std::vector<double>& args) { return std::atan(args[0]); };
    functions_["atan2"] = [](const std::vector<double>& args) { return std::atan2(args[0], args[1]); };

    // 双曲函数
    functions_["sinh"] = [](const std::vector<double>& args) { return std::sinh(args[0]); };
    functions_["cosh"] = [](const std::vector<double>& args) { return std::cosh(args[0]); };
    functions_["tanh"] = [](const std::vector<double>& args) { return std::tanh(args[0]); };
    functions_["asinh"] = [](const std::vector<double>& args) { return std::asinh(args[0]); };
    functions_["acosh"] = [](const std::vector<double>& args) { return std::acosh(args[0]); };
    functions_["atanh"] = [](const std::vector<double>& args) { return std::atanh(args[0]); };

    // 指数和对数
    functions_["exp"] = [](const std::vector<double>& args) { return std::exp(args[0]); };
    functions_["log"] = [](const std::vector<double>& args) { return std::log(args[0]); };
    functions_["ln"] = [](const std::vector<double>& args) { return std::log(args[0]); };
    functions_["log10"] = [](const std::vector<double>& args) { return std::log10(args[0]); };
    functions_["log2"] = [](const std::vector<double>& args) { return std::log2(args[0]); };

    // 幂和根
    functions_["sqrt"] = [](const std::vector<double>& args) { return std::sqrt(args[0]); };
    functions_["cbrt"] = [](const std::vector<double>& args) { return std::cbrt(args[0]); };
    functions_["pow"] = [](const std::vector<double>& args) { return std::pow(args[0], args[1]); };

    // 取整
    functions_["floor"] = [](const std::vector<double>& args) { return std::floor(args[0]); };
    functions_["ceil"] = [](const std::vector<double>& args) { return std::ceil(args[0]); };
    functions_["round"] = [](const std::vector<double>& args) { return std::round(args[0]); };
    functions_["frac"] = [](const std::vector<double>& args) { return args[0] - std::floor(args[0]); };

    // 其他
    functions_["abs"] = [](const std::vector<double>& args) { return std::abs(args[0]); };
    functions_["sign"] = [](const std::vector<double>& args) {
        if (args[0] > 0) return 1.0;
        if (args[0] < 0) return -1.0;
        return 0.0;
    };
    functions_["min"] = [](const std::vector<double>& args) { return std::min(args[0], args[1]); };
    functions_["max"] = [](const std::vector<double>& args) { return std::max(args[0], args[1]); };
    functions_["mod"] = [](const std::vector<double>& args) { return std::fmod(args[0], args[1]); };
}

void ExpressionEvaluator::registerFunction(const std::string& name, MathFunction func) {
    functions_[name] = std::move(func);
}

double ExpressionEvaluator::evaluate(const ExprNodePtr& node, const VariableContext& vars) {
    if (!node) {
        return std::nan("");
    }

    switch (node->type) {
        case NodeType::Number:
            return node->value;

        case NodeType::Variable: {
            auto it = vars.find(node->name);
            if (it != vars.end()) {
                return it->second;
            }
            throw std::runtime_error("未定义的变量: " + node->name);
        }

        case NodeType::BinaryOp: {
            double left = evaluate(node->left, vars);
            double right = evaluate(node->right, vars);

            if (node->op == "+") return left + right;
            if (node->op == "-") return left - right;
            if (node->op == "*") return left * right;
            if (node->op == "/") return left / right;
            if (node->op == "^") return std::pow(left, right);

            throw std::runtime_error("未知的运算符: " + node->op);
        }

        case NodeType::UnaryOp: {
            double operand = evaluate(node->left, vars);
            if (node->op == "-") return -operand;
            if (node->op == "+") return operand;
            throw std::runtime_error("未知的一元运算符: " + node->op);
        }

        case NodeType::Function: {
            std::vector<double> args;
            args.reserve(node->args.size());
            for (const auto& arg : node->args) {
                args.push_back(evaluate(arg, vars));
            }
            return evaluateFunction(node->name, args);
        }

        default:
            throw std::runtime_error("未知的节点类型");
    }
}

double ExpressionEvaluator::evaluateFunction(const std::string& name, const std::vector<double>& args) {
    auto it = functions_.find(name);
    if (it != functions_.end()) {
        return it->second(args);
    }
    throw std::runtime_error("未知的函数: " + name);
}

void ExpressionEvaluator::evaluateBatch(const ExprNodePtr& node,
                                        const std::vector<double>& xValues,
                                        std::vector<double>& results,
                                        const VariableContext& baseVars,
                                        const std::string& varName) {
    results.resize(xValues.size());
    VariableContext vars = baseVars;

    // 使用OpenMP并行计算（如果可用）
    #pragma omp parallel for if(xValues.size() > 1000)
    for (size_t i = 0; i < xValues.size(); ++i) {
        VariableContext localVars = vars;
        localVars[varName] = xValues[i];
        try {
            results[i] = evaluate(node, localVars);
        } catch (...) {
            results[i] = std::nan("");
        }
    }
}

void ExpressionEvaluator::evaluateGrid(const ExprNodePtr& node,
                                       const std::vector<double>& xValues,
                                       const std::vector<double>& yValues,
                                       std::vector<std::vector<double>>& results,
                                       const VariableContext& baseVars) {
    results.resize(yValues.size());
    for (auto& row : results) {
        row.resize(xValues.size());
    }

    #pragma omp parallel for if(xValues.size() * yValues.size() > 1000)
    for (size_t j = 0; j < yValues.size(); ++j) {
        VariableContext localVars = baseVars;
        localVars["y"] = yValues[j];
        for (size_t i = 0; i < xValues.size(); ++i) {
            localVars["x"] = xValues[i];
            try {
                results[j][i] = evaluate(node, localVars);
            } catch (...) {
                results[j][i] = std::nan("");
            }
        }
    }
}

void ExpressionEvaluator::evaluateVolume(const ExprNodePtr& node,
                                         const std::vector<double>& xValues,
                                         const std::vector<double>& yValues,
                                         const std::vector<double>& zValues,
                                         std::vector<double>& results,
                                         const VariableContext& baseVars) {
    size_t nx = xValues.size();
    size_t ny = yValues.size();
    size_t nz = zValues.size();
    results.resize(nx * ny * nz);

    #pragma omp parallel for if(nx * ny * nz > 1000)
    for (size_t k = 0; k < nz; ++k) {
        VariableContext localVars = baseVars;
        localVars["z"] = zValues[k];
        for (size_t j = 0; j < ny; ++j) {
            localVars["y"] = yValues[j];
            for (size_t i = 0; i < nx; ++i) {
                localVars["x"] = xValues[i];
                size_t idx = i + j * nx + k * nx * ny;
                try {
                    results[idx] = evaluate(node, localVars);
                } catch (...) {
                    results[idx] = std::nan("");
                }
            }
        }
    }
}

} // namespace ArchMaths
