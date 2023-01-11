#pragma once

#include "math.h"

auto translationTransform(Vec3 const &translation) -> Mat4;

auto projectionTransform(float fov_y_degrees, float aspect_ratio_xy) -> Mat4;
