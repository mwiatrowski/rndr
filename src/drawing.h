#pragma once

#include "framebuffer.h"
#include "math.h"
#include "mesh.h"

auto drawTriangle(FrameBuffer &fb, Triangle const &vertices) -> void;
auto drawMesh(FrameBuffer &fb, Mesh const &mesh, Mat4 const &transform) -> void;
