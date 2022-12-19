#include "transform.h"

auto translationTransform(Vec3 const &translation) -> Mat4 {
    auto const &[dx, dy, dz] = translation;
    return Mat4{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, dx, dy, dz, 1};
}
