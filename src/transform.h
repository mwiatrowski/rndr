#pragma once

#include "math.h"

auto translationTransform(Vec3 const &translation) -> Mat4;

auto projectionTransform(float fov_y_degrees, float aspect_ratio_xy, float z_near, float z_far) -> Mat4;
