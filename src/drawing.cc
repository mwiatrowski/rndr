#include "drawing.h"

#include <algorithm>

#include "math.h"

namespace {

auto remapToScreen(cv::Mat const &img, Triangle const &triangle) {
    auto scale = std::min(img.cols / 2, img.rows / 2);
    auto center = Vec2(img.cols / 2, img.rows / 2);

    auto result = Triangle{};
    for (int i = 0; i < ssize(triangle); ++i) {
        result[i] = Vec2{scale * triangle[i].x + center.x, scale * triangle[i].y + center.y};
    }
    return result;
}

auto setPixel(cv::Mat &img, int x, int y, cv::Vec3b color) -> void { img.at<cv::Vec3b>(y, x) = color; }

template <typename T1, typename... T2> auto min(T1 const &head, T2 const &...tail) {
    auto res = head;
    ((res = std::min(res, tail)), ...);
    return res;
}

template <typename T1, typename... T2> auto max(T1 const &head, T2 const &...tail) {
    auto res = head;
    ((res = std::max(res, tail)), ...);
    return res;
}

auto getTriangleBounds(cv::Mat const &img, Triangle const &triangle) -> cv::Rect {
    auto const &a = triangle[0];
    auto const &b = triangle[1];
    auto const &c = triangle[2];

    auto x1 = std::max(min(a.x, b.x, c.x), static_cast<float>(0));
    auto x2 = std::min(max(a.x, b.x, c.x), static_cast<float>(img.cols - 1));
    auto y1 = std::max(min(a.y, b.y, c.y), static_cast<float>(0));
    auto y2 = std::min(max(a.y, b.y, c.y), static_cast<float>(img.rows - 1));

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
    auto vertices_scaled = remapToScreen(img, vertices);

    auto bounds = getTriangleBounds(img, vertices_scaled);

    for (int y = bounds.y; y <= bounds.y + bounds.height; ++y) {
        assert(y >= 0 && y < img.rows);
        for (int x = bounds.x; x <= bounds.x + bounds.width; ++x) {
            assert(x >= 0 && x < img.cols);

            auto p = Vec2{static_cast<float>(x), static_cast<float>(y)};
            auto [u, v, w] = getBarycentricCoords(vertices_scaled, p);
            if (u < 0 || v < 0 || w < 0 || u > 1 || v > 1 || w > 1) {
                continue;
            }

            auto color =
                cv::Vec3b{static_cast<uint8_t>(255 * u), static_cast<uint8_t>(255 * v), static_cast<uint8_t>(255 * w)};
            setPixel(img, x, y, color);
        }
    }
}

auto drawMesh(cv::Mat &img, Mesh const &mesh) -> void {
    for (auto const &triangle : mesh) {
        drawTriangle(img, triangle);
    }
}
