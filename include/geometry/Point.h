#pragma once

#include "geometry/GeometryObject.h"

namespace ArchMaths {

class Point : public GeometryObject {
public:
    Point() = default;
    Point(double x, double y);

    void calculate() override;
    bool isValid() const override;

    double x() const { return x_; }
    double y() const { return y_; }
    void setX(double x) { x_ = x; }
    void setY(double y) { y_ = y; }

private:
    double x_ = 0.0;
    double y_ = 0.0;
};

} // namespace ArchMaths
