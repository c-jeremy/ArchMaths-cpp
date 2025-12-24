#pragma once

#include "geometry/GeometryObject.h"
#include "geometry/Point.h"
#include "geometry/Line.h"
#include "geometry/Circle.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace ArchMaths {

class GeometryManager {
public:
    GeometryManager() = default;

    // 添加几何对象
    void addPoint(const std::string& name, double x, double y);
    void addLine(const std::string& name, const std::string& p1Name, const std::string& p2Name);
    void addCircle(const std::string& name, const std::string& centerName, double radius);

    // 获取几何对象
    Point* getPoint(const std::string& name);
    Line* getLine(const std::string& name);
    Circle* getCircle(const std::string& name);

    // 重新计算所有依赖
    void recalculateAll();

    // 清空
    void clear();

private:
    std::unordered_map<std::string, std::unique_ptr<Point>> points_;
    std::unordered_map<std::string, std::unique_ptr<Line>> lines_;
    std::unordered_map<std::string, std::unique_ptr<Circle>> circles_;
};

} // namespace ArchMaths
