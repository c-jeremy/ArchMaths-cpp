#include "geometry/GeometryManager.h"

namespace ArchMaths {

void GeometryManager::addPoint(const std::string& name, double x, double y) {
    auto point = std::make_unique<Point>(x, y);
    point->name = name;
    points_[name] = std::move(point);
}

void GeometryManager::addLine(const std::string& name, const std::string& p1Name, const std::string& p2Name) {
    auto* p1 = getPoint(p1Name);
    auto* p2 = getPoint(p2Name);
    if (p1 && p2) {
        auto line = std::make_unique<Line>(*p1, *p2);
        line->name = name;
        lines_[name] = std::move(line);
    }
}

void GeometryManager::addCircle(const std::string& name, const std::string& centerName, double radius) {
    auto* center = getPoint(centerName);
    if (center) {
        auto circle = std::make_unique<Circle>(*center, radius);
        circle->name = name;
        circles_[name] = std::move(circle);
    }
}

Point* GeometryManager::getPoint(const std::string& name) {
    auto it = points_.find(name);
    return it != points_.end() ? it->second.get() : nullptr;
}

Line* GeometryManager::getLine(const std::string& name) {
    auto it = lines_.find(name);
    return it != lines_.end() ? it->second.get() : nullptr;
}

Circle* GeometryManager::getCircle(const std::string& name) {
    auto it = circles_.find(name);
    return it != circles_.end() ? it->second.get() : nullptr;
}

void GeometryManager::recalculateAll() {
    for (auto& [name, line] : lines_) {
        line->calculate();
    }
    for (auto& [name, circle] : circles_) {
        circle->calculate();
    }
}

void GeometryManager::clear() {
    points_.clear();
    lines_.clear();
    circles_.clear();
}

} // namespace ArchMaths
