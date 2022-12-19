#include "drawing.h"

#include <algorithm>

#include "math.h"

namespace {

using Triangle2D = std::array<Vec2, 3>;

auto operator*(Vertex const &vertex, Mat4 const &transform) -> Vertex {
    auto const &[x, y, z] = vertex;
    auto v2 = Vec4{x, y, z, 1} * transform;

    auto const &x2 = v2[0];
    auto const &y2 = v2[1];
    auto const &z2 = v2[2];
    auto const &w2 = v2[3];

    return w2 == 0 ? Vertex{0, 0, 0} : Vertex{x2 / w2, y2 / w2, z2 / w2};
}

auto operator*(Triangle const &triangle, Mat4 const &transform) -> Triangle {
    auto const &[a, b, c] = triangle;
    return Triangle{a * transform, b * transform, c * transform};
}

auto remapToScreen(cv::Mat const &img, Triangle const &triangle) -> Triangle2D {
    auto scale = std::min(img.cols / 2, img.rows / 2);
    auto center = Vec2(img.cols / 2, img.rows / 2);

    auto result = Triangle2D{};
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

auto getTriangleBounds(cv::Mat const &img, Triangle2D const &triangle) -> cv::Rect {
    auto const &a = triangle[0];
    auto const &b = triangle[1];
    auto const &c = triangle[2];

    auto x1 = std::max(min(a.x, b.x, c.x), static_cast<float>(0));
    auto x2 = std::min(max(a.x, b.x, c.x), static_cast<float>(img.cols - 1));
    auto y1 = std::max(min(a.y, b.y, c.y), static_cast<float>(0));
    auto y2 = std::min(max(a.y, b.y, c.y), static_cast<float>(img.rows - 1));

    return cv::Rect(x1, y1, x2 - x1, y2 - y1);
}

auto getTriangleArea(Triangle2D const &triangle) -> float {
    auto const &[a, b, c] = triangle;
    return cross(c - b, b - a);
}

auto getBarycentricCoords(Triangle2D const &triangle, Vec2 p) -> Vec3 {
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

auto drawTriangle(cv::Mat &img, Triangle const &vertices, Mat4 const &transform) -> void {
    auto vertices_transformed = vertices * transform;
    auto vertices_scaled = remapToScreen(img, vertices_transformed);

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

auto drawMesh(cv::Mat &img, Mesh const &mesh, Mat4 const &transform) -> void {
    for (auto const &triangle : mesh) {
        drawTriangle(img, triangle, transform);
    }
}
