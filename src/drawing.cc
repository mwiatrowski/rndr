#include "drawing.h"

#include <algorithm>
#include <cmath>

#include "math.h"

namespace {

using Triangle2D = std::array<Vec2, 3>;

auto operator*(Vertex const &vertex, Mat4 const &transform) -> Vec3 {
    auto const &[x, y, z] = vertex.position;
    auto v2 = Vec4{x, y, z, 1} * transform;

    auto const &x2 = v2[0];
    auto const &y2 = v2[1];
    auto const &z2 = v2[2];
    auto const &w2 = v2[3];

    return w2 == 0 ? Vec3{0, 0, 0} : Vec3{x2 / w2, y2 / w2, z2 / w2};
}

using Triangle = std::array<Vertex, 3>;

auto remapToScreen(cv::Mat const &img, Triangle const &triangle) -> Triangle2D {
    auto scale = Vec2(img.cols / 2, img.rows / 2);
    auto center = Vec2(img.cols / 2, img.rows / 2);

    auto result = Triangle2D{};
    for (int i = 0; i < ssize(triangle); ++i) {
        auto const &[x, y, z] = triangle[i].position;
        result[i] = Vec2{scale.x * x + center.x, scale.y * y + center.y};
    }
    return result;
}

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

auto makeInterpolatingFunction(Triangle2D const &points, std::array<float, 3> const &values) {
    auto const &[x1, y1] = points[0];
    auto const &[x2, y2] = points[1];
    auto const &[x3, y3] = points[2];
    auto const &[v1, v2, v3] = values;

    auto x_21 = x2 - x1;
    auto x_13 = x1 - x3;
    auto y_21 = y2 - y1;
    auto y_13 = y1 - y3;
    auto v_21 = v2 - v1;
    auto v_13 = v1 - v3;

    float A, B, C;

    auto area = (x_21 * y_13 - x_13 * y_21);
    if (area != 0) {
        A = (v_21 * y_13 - v_13 * y_21) / area;
        B = (v_13 * x_21 - v_21 * x_13) / area;
        C = v1 - A * x1 - B * y1;
    } else {
        // Special case: degenerate triangle.
        auto s1 = x1 + y1;
        auto s2 = x2 + y2;
        auto s3 = x3 + y3;

        Vec2 p1, p2;
        p1 = {s1, v1};
        if (std::abs(s1 - s2) > std::abs(s1 - s3)) {
            p2 = {s2, v2};
        } else {
            p2 = {s3, v3};
        }

        if (p1.x != p2.x) {
            A = (p2.y - p1.y) / (p2.x - p1.x);
            B = A;
            C = (p1.y * p2.x - p2.y * p1.x) / (p2.x - p1.x);
        } else {
            // Even more special case: some points overlap.
            A = 0;
            B = 0;
            C = (v1 + v2 + v3) / 3;
        }
    }

    return [A, B, C](float x, float y) { return A * x + B * y + C; };
}

auto drawTriangle(FrameBuffer &fb, Triangle const &vertices) -> void {
    auto vertices_scaled = remapToScreen(fb.render_target, vertices);

    auto bounds = getTriangleBounds(fb.render_target, vertices_scaled);

    auto inv_depth = Vec3{vertices[0].position.z, vertices[1].position.z, vertices[2].position.z};
    auto get_inv_d = makeInterpolatingFunction(vertices_scaled, {inv_depth.x, inv_depth.y, inv_depth.z});

    // TODO: This is not quite correct, triangle needs to be clipped so that the part in front of the camera can be
    // rendered.
    if (inv_depth.x < 0 || inv_depth.y < 0 || inv_depth.z < 0) {
        return;
    }

    // Barycentric coordinates
    auto get_u = makeInterpolatingFunction(vertices_scaled, {inv_depth.x, 0, 0});
    auto get_v = makeInterpolatingFunction(vertices_scaled, {0, inv_depth.y, 0});

    for (int y = bounds.y; y <= bounds.y + bounds.height; ++y) {
        assert(y >= 0 && y < fb.render_target.rows);
        for (int x = bounds.x; x <= bounds.x + bounds.width; ++x) {
            assert(x >= 0 && x < fb.render_target.cols);

            auto inverse_depth = get_inv_d(x, y);
            auto depth = 1.0 / inverse_depth;

            auto u = get_u(x, y) * depth;
            auto v = get_v(x, y) * depth;
            auto w = 1.0 - u - v;

            if (u < 0 || v < 0 || w < 0 || u > 1 || v > 1 || w > 1) {
                continue;
            }

            auto tx_u =
                vertices[0].texture_coords.x * u + vertices[1].texture_coords.x * v + vertices[2].texture_coords.x * w;
            auto tx_v =
                vertices[0].texture_coords.y * u + vertices[1].texture_coords.y * v + vertices[2].texture_coords.y * w;

            auto checkerboard = [](float a, float b) -> uint8_t {
                auto a2 = static_cast<int>(std::round(256 * a));
                auto a3 = a2 / 8;
                auto b2 = static_cast<int>(std::round(256 * b));
                auto b3 = b2 / 8;
                return ((a3 ^ b3) & 1) ? 255 : 0;
            };

            auto color = cv::Vec3b{static_cast<uint8_t>(std::round(255 * tx_u)),
                                   static_cast<uint8_t>(std::round(255 * tx_v)), checkerboard(tx_u, tx_v)};

            setPixel(fb, x, y, inverse_depth, color);
        }
    }
}

} // namespace

auto drawMesh(FrameBuffer &fb, Mesh const &mesh, Mat4 const &transform) -> void {
    assert(isMeshValid(mesh));

    auto vertices_transformed = std::vector<Vertex>{};
    vertices_transformed.reserve(mesh.vertices.size());
    for (auto const &v : mesh.vertices) {
        vertices_transformed.push_back(Vertex{.position = v * transform, .texture_coords = v.texture_coords});
    }

    auto const n = std::ssize(mesh.indices);
    for (auto i = 0; i < n; i += 3) {
        auto triangle = Triangle{vertices_transformed[mesh.indices[i]], vertices_transformed[mesh.indices[i + 1]],
                                 vertices_transformed[mesh.indices[i + 2]]};
        drawTriangle(fb, std::move(triangle));
    }
}
