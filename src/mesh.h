#pragma once

#include <array>
#include <vector>

#include "math.h"

struct Vertex {
    Vec3 position;
    Vec2 texture_coords;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<int> indices;
};

auto isMeshValid(Mesh const &mesh) -> bool;
