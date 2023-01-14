#include "mesh.h"

#include <algorithm>

auto isMeshValid(Mesh const &mesh) -> bool {
    if (mesh.indices.size() % 3 != 0)
        return false;

    auto is_index_ok = [n = std::ssize(mesh.vertices)](int index) { return index >= 0 && index < n; };
    return std::ranges::all_of(mesh.indices, is_index_ok);
}
