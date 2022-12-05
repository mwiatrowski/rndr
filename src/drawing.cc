#include "drawing.h"

#include <algorithm>

#include "math.h"

namespace {

auto setPixel(cv::Mat &img, int x, int y, cv::Vec3b color) -> void { img.at<cv::Vec3b>(y, x) = color; }

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

auto getTriangleArea(Triangle const &triangle) -> float {
    auto const &[a, b, c] = triangle;
    return cross(c - b, b - a);
}

auto getBarycentricCoords(Triangle const &triangle, Vec2 p) -> Vec3 {
    auto const &[a, b, c] = triangle;

    auto area_abc = getTriangleArea(triangle);
    if (area_abc == 0.0) {
        return Vec3(3.0, -1.0, -1.0);
    }

    auto area_pbc = getTriangleArea({p, b, c});
    auto area_pca = getTriangleArea({p, c, a});
    auto area_pab = getTriangleArea({p, a, b});

    return Vec3{area_pbc / area_abc, area_pca / area_abc, area_pab / area_abc};
}

} // namespace

auto clearImage(cv::Mat &img, cv::Vec3b color) -> void { img.setTo(color); }

auto drawTriangle(cv::Mat &img, Triangle const &vertices) -> void {
    auto bounds = getTriangleBounds(vertices);

    for (int y = bounds.y; y <= bounds.y + bounds.height; ++y) {
        for (int x = bounds.x; x <= bounds.x + bounds.width; ++x) {
            auto p = Vec2{static_cast<float>(x), static_cast<float>(y)};
            auto [u, v, w] = getBarycentricCoords(vertices, p);
            if (u < 0 || v < 0 || w < 0 || u > 1 || v > 1 || w > 1) {
                continue;
            }

            auto color =
                cv::Vec3b{static_cast<uint8_t>(255 * u), static_cast<uint8_t>(255 * v), static_cast<uint8_t>(255 * w)};
            setPixel(img, x, y, color);
        }
    }
}
