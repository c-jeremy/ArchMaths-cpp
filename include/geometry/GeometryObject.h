#pragma once

#include "math/MathTypes.h"
#include <memory>

namespace ArchMaths {

// 几何对象基类
class GeometryObject {
public:
    virtual ~GeometryObject() = default;

    virtual void calculate() = 0;
    virtual bool isValid() const = 0;

    std::string name;
    Color color;
    float thickness = 3.0f;
    bool visible = true;
};

} // namespace ArchMaths
