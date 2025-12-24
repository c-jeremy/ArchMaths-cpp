#pragma once

#include "geometry/GeometryObject.h"
#include "geometry/Point.h"

namespace ArchMaths {

class Line : public GeometryObject {
public:
    Line() = default;
    Line(const Point& p1, const Point& p2);

    void calculate() override;
    bool isValid() const override;

    const Point& p1() const { return p1_; }
    const Point& p2() const { return p2_; }

    // 直线方程 ax + by + c = 0
    double a() const { return a_; }
    double b() const { return b_; }
    double c() const { return c_; }

private:
    Point p1_, p2_;
    double a_ = 0.0, b_ = 0.0, c_ = 0.0;
};

} // namespace ArchMaths
