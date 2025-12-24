#include "geometry/Point.h"

namespace ArchMaths {

Point::Point(double x, double y) : x_(x), y_(y) {}

void Point::calculate() {
    // 点不需要额外计算
}

bool Point::isValid() const {
    return std::isfinite(x_) && std::isfinite(y_);
}

} // namespace ArchMaths
