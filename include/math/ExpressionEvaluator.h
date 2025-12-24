#pragma once

#include "math/MathTypes.h"
#include <unordered_map>
#include <functional>

namespace ArchMaths {

class ExpressionEvaluator {
public:
    ExpressionEvaluator();

    // 求值表达式
    double evaluate(const ExprNodePtr& node, const VariableContext& vars);

    // 批量求值（用于绘图，性能优化）
    void evaluateBatch(const ExprNodePtr& node,
                       const std::vector<double>& xValues,
                       std::vector<double>& results,
                       const VariableContext& baseVars,
                       const std::string& varName = "x");

    // 2D网格求值（用于3D曲面 z=f(x,y)）
    void evaluateGrid(const ExprNodePtr& node,
                      const std::vector<double>& xValues,
                      const std::vector<double>& yValues,
                      std::vector<std::vector<double>>& results,
                      const VariableContext& baseVars);

    // 3D体积求值（用于隐式3D曲面 f(x,y,z)=0）
    void evaluateVolume(const ExprNodePtr& node,
                        const std::vector<double>& xValues,
                        const std::vector<double>& yValues,
                        const std::vector<double>& zValues,
                        std::vector<double>& results,
                        const VariableContext& baseVars);

    // 注册自定义函数
    void registerFunction(const std::string& name, MathFunction func);

private:
    double evaluateFunction(const std::string& name, const std::vector<double>& args);

    FunctionRegistry functions_;
    void initBuiltinFunctions();
};

} // namespace ArchMaths
