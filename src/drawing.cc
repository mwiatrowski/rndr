#include "drawing.h"

#include <algorithm>

#include "math.h"

#include <iostream>

namespace {

auto getTriangleBounds(Triangle const &triangle) -> cv::Rect {
    auto const &a = triangle[0];
    auto const &b = triangle[1];
    auto const &c = triangle[2];

    auto x1 = std::min(a.x, std::min(b.x, c.x));
    auto x2 = std::max(a.x, std::max(b.x, c.x));
    auto y1 = std::min(a.y, std::min(b.y, c.y));
    auto y2 = std::max(a.y, std::max(b.y, c.y));

    return cv::Rect(x1, y1, x2 - x1, y2 - y1);
}

auto getBarycentricCoords(Triangle const &triangle, Vec2 p) -> Vec3 {
    auto const &[a, b, c] = triangle;

    if (cross(p - c, b - c) == 0.0) {
        return Vec3{1.0, 0.0, 0.0};
    }

    auto u = cross(p - c, b - c) / cross(a - c, b - c);
    auto v = cross(p - a, c - a) / cross(b - a, c - a);
    auto w = cross(p - b, a - b) / cross(c - b, a - b);

    return Vec3{u, v, w};
}

} // namespace

auto clearImage(cv::Mat &img, cv::Vec3b color) -> void { img.setTo(color); }

auto drawTriangle(cv::Mat &img, Triangle const &vertices, cv::Vec3b color) -> void {
    auto bounds = getTriangleBounds(vertices);

    for (int row = bounds.y; row <= bounds.y + bounds.height; ++row) {
        for (int col = bounds.x; col <= bounds.x + bounds.width; ++col) {
            auto p = Vec2{static_cast<float>(col), static_cast<float>(row)};
            auto [u, v, w] = getBarycentricCoords(vertices, p);
            if (u < 0 || v < 0 || w < 0) {
                continue;
            }

            img.at<cv::Vec3b>(row, col) = color;
        }
    }
}
