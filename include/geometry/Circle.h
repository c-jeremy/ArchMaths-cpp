#pragma once

#include "geometry/GeometryObject.h"
#include "geometry/Point.h"

namespace ArchMaths {

class Circle : public GeometryObject {
public:
    Circle() = default;
    Circle(const Point& center, double radius);

    void calculate() override;
    bool isValid() const override;

    const Point& center() const { return center_; }
    double radius() const { return radius_; }

private:
    Point center_;
    double radius_ = 0.0;
};

} // namespace ArchMaths
