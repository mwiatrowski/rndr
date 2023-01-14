#include "math.h"

#include <cmath>

auto operator-(Vec2 const &lhs, Vec2 const &rhs) -> Vec2 { return Vec2{lhs.x - rhs.x, lhs.y - rhs.y}; }

auto cross(Vec2 const &lhs, Vec2 const &rhs) -> float { return lhs.x * rhs.y - lhs.y * rhs.x; }

auto operator<<(std::ostream &stream, Vec3 const &vec) -> std::ostream & {
    stream << "{" << vec.x << ", " << vec.y << ", " << vec.z << "}";
    return stream;
}

auto operator+(Vec3 const &lhs, Vec3 const &rhs) -> Vec3 { return Vec3{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z}; }

auto operator-(Vec3 const &vec) -> Vec3 {
    auto const &[x, y, z] = vec;
    return Vec3{-x, -y, -z};
}

auto operator-(Vec3 const &lhs, Vec3 const &rhs) -> Vec3 { return Vec3{lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z}; }

auto operator*(float scalar, Vec3 const &vec) -> Vec3 {
    auto const &[x, y, z] = vec;
    return Vec3{scalar * x, scalar * y, scalar * z};
}

auto dot(Vec3 const &lhs, Vec3 const &rhs) -> float { return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z; }

auto cross(Vec3 const &lhs, Vec3 const &rhs) -> Vec3 {
    auto const &[x1, y1, z1] = lhs;
    auto const &[x2, y2, z2] = rhs;
    return Vec3{
        y1 * z2 - z1 * y2, //
        z1 * x2 - x1 * z2, //
        x1 * y2 - y1 * x2  //
    };
}

auto norm(Vec3 const &vec) -> float {
    auto const &[x, y, z] = vec;
    return std::sqrt(x * x + y * y + z * z);
}

auto normalize(Vec3 const &vec) -> Vec3 {
    auto vec_norm = norm(vec);
    return (vec_norm != 0) ? ((1 / vec_norm) * vec) : Vec3{1, 0, 0};
}
