#include "drawing.h"

#include <algorithm>
#include <cmath>

#include "math.h"

namespace {

auto operator*(Vertex const &vertex, Mat4 const &transform) -> Vec3 {
    auto const &[x, y, z] = vertex.position;
    auto v2 = Vec4{x, y, z, 1} * transform;

    auto const &x2 = v2[0];
    auto const &y2 = v2[1];
    auto const &z2 = v2[2];
    auto const &w2 = v2[3];

    return w2 == 0 ? Vec3{0, 0, 0} : Vec3{x2 / w2, y2 / w2, z2 / w2};
}

struct Vec2i {
    int64_t x;
    int64_t y;
};

using Triangle = std::array<Vertex, 3>;

auto remapToScreen(cv::Mat const &img, Triangle const &triangle) {
    auto half = Vec2((img.cols - 1) / 2.f, (img.rows - 1) / 2.f);

    std::array<Vec2i, 3> result;
    for (int i = 0; i < ssize(triangle); ++i) {
        auto const &[px, py, pz] = triangle[i].position;
        result[i].x = static_cast<int64_t>(std::round(half.x * px + half.x));
        result[i].y = static_cast<int64_t>(std::round(half.y * py + half.y));
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

struct Rect {
    int64_t x1, y1, x2, y2;
};

auto getTriangleBounds(cv::Mat const &img, std::array<Vec2i, 3> const &triangle) -> Rect {
    auto const &[a, b, c] = triangle;

    auto x1 = std::max(min(a.x, b.x, c.x), 0L);
    auto x2 = std::min(max(a.x, b.x, c.x), int64_t{img.cols - 1});
    auto y1 = std::max(min(a.y, b.y, c.y), 0L);
    auto y2 = std::min(max(a.y, b.y, c.y), int64_t{img.rows - 1});

    return Rect{.x1 = x1, .y1 = y1, .x2 = x2, .y2 = y2};
}

struct EdgeFunction {
  private:
    int64_t base_, dx_, dy_;

  public:
    EdgeFunction(int64_t base, int64_t dx, int64_t dy) : base_(base), dx_(dx), dy_(dy) {}

    auto operator()(Vec2i const &p) -> int64_t { return base_ + dx_ * p.x + dy_ * p.y; }
};

auto makeEdgeFunction(Vec2i const &v1, Vec2i const &v2) {
    auto base = v2.x * v1.y - v2.y * v1.x;
    auto dx = v2.y - v1.y;
    auto dy = v1.x - v2.x;
    return EdgeFunction(base, dx, dy);
}

auto drawTriangle(FrameBuffer &fb, Triangle const &vertices) -> void {
    auto screen_space = remapToScreen(fb.render_target, vertices);
    auto const &[ss_a, ss_b, ss_c] = screen_space;

    auto screen_space_area = std::invoke([ss_a, ss_b, ss_c] {
        auto cb_x = ss_c.x - ss_b.x;
        auto cb_y = ss_c.y - ss_b.y;
        auto ba_x = ss_b.x - ss_a.x;
        auto ba_y = ss_b.y - ss_a.y;
        return cb_x * ba_y - cb_y * ba_x;
    });

    if (screen_space_area == 0)
        return;

    // Back-face culling
    if (screen_space_area < 0)
        return;

    auto get_u = makeEdgeFunction(ss_b, ss_c);
    auto get_v = makeEdgeFunction(ss_c, ss_a);
    auto get_w = makeEdgeFunction(ss_a, ss_b);

    auto inv_depth = std::array{vertices[0].position.z, vertices[1].position.z, vertices[2].position.z};

    // TODO: This is not quite correct, triangle needs to be clipped so that the part in front of the camera can be
    // rendered.
    if (inv_depth[0] < 0 || inv_depth[1] < 0 || inv_depth[2] < 0) {
        return;
    }

    auto bounds = getTriangleBounds(fb.render_target, screen_space);

    for (int y = bounds.y1; y <= bounds.y2; ++y) {
        assert(y >= 0 && y < fb.render_target.rows);

        for (int x = bounds.x1; x <= bounds.x2; ++x) {
            assert(x >= 0 && x < fb.render_target.cols);

            auto u = get_u(Vec2i{x, y});
            auto v = get_v(Vec2i{x, y});
            auto w = get_w(Vec2i{x, y});

            // TODO: This also does back-face culling. Might want to change that.
            if (u < 0 || v < 0 || w < 0)
                continue;

            auto inverse_depth = (u * inv_depth[0] + v * inv_depth[1] + w * inv_depth[2]) / (u + v + w);
            auto depth = 1.f / inverse_depth;

            auto nu = u / float(u + v + w);
            auto nv = v / float(u + v + w);
            auto nw = w / float(u + v + w);

            auto interpolate = [&](float a, float b, float c) -> float { return (a * nu) + (b * nv) + (c * nw); };
            auto interpolate_perspective = [&](float a, float b, float c) -> float {
                auto az = a * inv_depth[0];
                auto bz = b * inv_depth[1];
                auto cz = c * inv_depth[2];
                return (interpolate(az, bz, cz) * depth);
            };

            auto tx_u = interpolate_perspective(vertices[0].texture_coords.x, vertices[1].texture_coords.x,
                                                vertices[2].texture_coords.x);
            auto tx_v = interpolate_perspective(vertices[0].texture_coords.y, vertices[1].texture_coords.y,
                                                vertices[2].texture_coords.y);

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

        drawTriangle(fb, triangle);
    }
}
