#pragma once

#include <array>
#include <vector>

#include "math.h"

using Vertex = Vec3;
using Triangle = std::array<Vertex, 3>;

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<int> indices;
};

auto isMeshValid(Mesh const &mesh) -> bool;
