#pragma once

#include <array>
#include <vector>

#include <opencv2/opencv.hpp>

#include "framebuffer.h"
#include "math.h"

using Vertex = Vec3;
using Triangle = std::array<Vertex, 3>;
using Mesh = std::vector<Triangle>;

auto drawTriangle(FrameBuffer &fb, Triangle const &vertices, Mat4 const &transform) -> void;
auto drawMesh(FrameBuffer &fb, Mesh const &mesh, Mat4 const &transform) -> void;
