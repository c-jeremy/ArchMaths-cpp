#include "geometry/Line.h"
#include <cmath>

namespace ArchMaths {

Line::Line(const Point& p1, const Point& p2) : p1_(p1), p2_(p2) {
    calculate();
}

void Line::calculate() {
    // 计算直线方程 ax + by + c = 0
    a_ = p2_.y() - p1_.y();
    b_ = p1_.x() - p2_.x();
    c_ = p2_.x() * p1_.y() - p1_.x() * p2_.y();

    // 归一化
    double norm = std::sqrt(a_ * a_ + b_ * b_);
    if (norm > 1e-10) {
        a_ /= norm;
        b_ /= norm;
        c_ /= norm;
    }
}

bool Line::isValid() const {
    return p1_.isValid() && p2_.isValid() &&
           (std::abs(p1_.x() - p2_.x()) > 1e-10 ||
            std::abs(p1_.y() - p2_.y()) > 1e-10);
}

} // namespace ArchMaths
