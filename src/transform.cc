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

auto projectionTransform(float fov_y_degrees, float aspect_ratio_xy, float z_near, float z_far) -> Mat4 {
    auto fov_rad = fov_y_degrees * std::numbers::pi / 180;
    float y = 1.0 / std::tan(fov_rad / 2.0);
    float x = y / aspect_ratio_xy;
    float z = (z_near + z_far) / (z_far - z_near);
    float w = -(2 * z_far * z_near) / (z_far - z_near);

    return Mat4{
        x, 0, 0, 0, //
        0, y, 0, 0, //
        0, 0, z, 1, //
        0, 0, w, 0  //
    };
}
