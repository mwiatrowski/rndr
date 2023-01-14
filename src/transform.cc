#include "transform.h"

#include <cmath>
#include <numbers>

auto translationTransform(Vec3 const &translation) -> Mat4 {
    auto const &[dx, dy, dz] = translation;
    return Mat4{
        1,  0,  0,  0, //
        0,  1,  0,  0, //
        0,  0,  1,  0, //
        dx, dy, dz, 1  //
    };
}

auto lookAt(Vec3 const &eye, Vec3 const &at, Vec3 const &up) -> Mat4 {
    auto look_dir = normalize(at - eye);
    auto const &[x_d, y_d, z_d] = look_dir;

    auto up_normalized = normalize(up);
    auto actual_up = normalize(up_normalized - dot(look_dir, up_normalized) * look_dir);
    auto const &[x_u, y_u, z_u] = actual_up;

    auto right = cross(look_dir, actual_up);
    auto const &[x_r, y_r, z_r] = right;

    auto translation = translationTransform(-eye);
    auto rotation = Mat4{
        x_r, -x_u, x_d, 0, //
        y_r, -y_u, y_d, 0, //
        z_r, -z_u, z_d, 0, //
        0,   0,    0,   1  //
    };
    return translation * rotation;
}

auto projectionTransform(float fov_y_degrees, float aspect_ratio_xy) -> Mat4 {
    auto fov_rad = fov_y_degrees * std::numbers::pi / 180;
    float y = 1.0 / std::tan(fov_rad / 2.0);
    float x = y / aspect_ratio_xy;

    return Mat4{
        x, 0, 0, 0, //
        0, y, 0, 0, //
        0, 0, 0, 1, //
        0, 0, 1, 0  //
    };
}
