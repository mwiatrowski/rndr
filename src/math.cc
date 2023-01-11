#include "math.h"

auto operator-(Vec2 const &lhs, Vec2 const &rhs) -> Vec2 { return Vec2{lhs.x - rhs.x, lhs.y - rhs.y}; }

auto cross(Vec2 const &lhs, Vec2 const &rhs) -> float { return lhs.x * rhs.y - lhs.y * rhs.x; }

auto dot(Vec3 const &lhs, Vec3 const &rhs) -> float { return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z; }
