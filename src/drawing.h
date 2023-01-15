#pragma once

#include "framebuffer.h"
#include "math.h"
#include "mesh.h"

auto drawMesh(FrameBuffer &fb, Mesh const &mesh, Mat4 const &transform) -> void;
