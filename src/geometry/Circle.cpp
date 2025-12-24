#include "geometry/Circle.h"
#include <cmath>

namespace ArchMaths {

Circle::Circle(const Point& center, double radius)
    : center_(center), radius_(radius) {}

void Circle::calculate() {
    // 圆不需要额外计算
}

bool Circle::isValid() const {
    return center_.isValid() && std::isfinite(radius_) && radius_ > 0;
}

} // namespace ArchMaths
