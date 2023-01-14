#pragma once

#include "math.h"

auto translationTransform(Vec3 const &translation) -> Mat4;

auto lookAt(Vec3 const &eye, Vec3 const &at, Vec3 const &up) -> Mat4;

auto projectionTransform(float fov_y_degrees, float aspect_ratio_xy) -> Mat4;
