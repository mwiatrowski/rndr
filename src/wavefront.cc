#include "wavefront.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <fstream>
#include <iostream>
#include <optional>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>

#include "mesh.h"

namespace {

bool isNotWhitespace(char ch) { return (std::isspace(static_cast<unsigned char>(ch)) == 0); }

std::string trimStr(std::string const &input) {
    ssize_t const n = std::ssize(input);
    ssize_t left_cut, right_cut;

    for (left_cut = 0; left_cut < n; ++left_cut) {
        if (isNotWhitespace(input[left_cut]))
            break;
    }

    for (right_cut = n; right_cut > left_cut; --right_cut) {
        if (isNotWhitespace(input[right_cut - 1]))
            break;
    }

    return std::string{input.begin() + left_cut, input.begin() + right_cut};
}

auto splitStr(std::string const &input, char separator, bool want_empty) {
    auto result = std::vector<std::string_view>{};

    size_t pStart = 0;
    size_t pEnd = input.find(separator);
    while (pEnd != std::string::npos) {
        if (pEnd - pStart > 0 || want_empty) {
            result.push_back(std::string_view{input.begin() + pStart, input.begin() + pEnd});
        }
        pStart = pEnd + 1;
        pEnd = input.find(separator, pStart);
    }
    result.push_back(std::string_view{input.begin() + pStart, input.end()});

    return result;
}

// Input must be trimmed
bool tryParseVertex(std::string const &line, std::vector<Vec3> &vertices) {
    if (!line.starts_with("v "))
        return false;

    std::string prefix;
    float x, y, z;
    auto stream = std::stringstream{line};
    stream >> prefix >> x >> y >> z;

    if (stream.fail() || !stream.eof())
        return false;

    // Flip X and Y axes to match what the rest of the engine expects.
    vertices.emplace_back(Vec3{-x, -y, z});
    return true;
}

// Input must be trimmed
bool tryParseTextureCoord(std::string const &line, std::vector<Vec2> &texture_coords) {
    if (!line.starts_with("vt "))
        return false;

    auto stream = std::stringstream{line};

    std::string prefix;
    stream >> prefix;

    float u, v;
    stream >> u >> v;

    while (!stream.fail() && !stream.eof()) {
        float ignored;
        stream >> ignored;
    }

    if (stream.fail() || !stream.eof())
        return false;

    texture_coords.emplace_back(Vec2{u, v});
    return true;
}

bool tryParseNormal(std::string const &line) {
    if (!line.starts_with("vn "))
        return false;

    // Ignore normals.
    return true;
}

struct IndexedVertex {
    int vertex_idx;
    std::optional<int> coords_idx;
};

auto operator<(IndexedVertex const &lhs, IndexedVertex const &rhs) -> bool {
    if (lhs.vertex_idx != rhs.vertex_idx)
        return lhs.vertex_idx < rhs.vertex_idx;

    if (lhs.coords_idx.has_value() != rhs.coords_idx.has_value())
        return !lhs.coords_idx.has_value();

    if (lhs.coords_idx.has_value())
        return lhs.coords_idx.value() < rhs.coords_idx.value();

    return false;
};

auto operator==(IndexedVertex const &lhs, IndexedVertex const &rhs) -> bool {
    return (lhs.vertex_idx == rhs.vertex_idx) && (lhs.coords_idx.has_value() == rhs.coords_idx.has_value()) &&
           (lhs.coords_idx.has_value() ? (lhs.coords_idx.value() == rhs.coords_idx.value()) : true);
};

// Input must be trimmed
bool tryParseVertexDescription(std::string description, std::vector<IndexedVertex> &indexed) {
    auto components = splitStr(description, '/', true);
    if (components.size() == 0)
        return false;

    int vertex_idx;
    auto v_idx_stream = std::stringstream{std::string{components[0]}} >> vertex_idx;
    if (v_idx_stream.fail())
        return false;

    auto maybe_coords_idx = std::optional<int>{};
    if (components.size() >= 2 && components[1].length() > 0) {
        int coords_idx;
        auto c_idx_stream = std::stringstream{std::string{components[1]}} >> coords_idx;
        if (c_idx_stream.fail())
            return false;
        maybe_coords_idx = coords_idx;
    }

    indexed.emplace_back(IndexedVertex{.vertex_idx = vertex_idx, .coords_idx = maybe_coords_idx});
    return true;
}

// Input must be trimmed
bool tryParseFace(std::string const &line, std::vector<IndexedVertex> &indexed) {
    if (!line.starts_with("f "))
        return false;

    auto indices_this_line = std::vector<IndexedVertex>{};

    auto split_line = splitStr(line, ' ', false);
    for (auto vertex_description : std::ranges::drop_view{split_line, 1}) {
        if (!tryParseVertexDescription(std::string{vertex_description}, indices_this_line))
            return false;
    }

    if (indices_this_line.size() == 3) {
        indexed.insert(indexed.end(), indices_this_line.begin(), indices_this_line.end());
    } else if (indices_this_line.size() == 4) {
        indexed.push_back(indices_this_line[0]);
        indexed.push_back(indices_this_line[1]);
        indexed.push_back(indices_this_line[2]);
        indexed.push_back(indices_this_line[0]);
        indexed.push_back(indices_this_line[2]);
        indexed.push_back(indices_this_line[3]);
    } else {
        return false;
    }

    return true;
}

auto tryParseLine(std::string const &line, std::vector<Vec3> &vertices, std::vector<Vec2> &texture_coords,
                  std::vector<IndexedVertex> &indexed) {
    auto log_error = [](auto const &str) {
        std::cerr << "Skipping line '" << str << "'" << std::endl;
        return false;
    };

    tryParseVertex(line, vertices) || tryParseTextureCoord(line, texture_coords) || tryParseNormal(line) ||
        tryParseFace(line, indexed) || log_error(line);
}

auto deduplicateIndexedVertices(std::vector<IndexedVertex> const &indexed) {
    auto indices_sorted = indexed;
    std::ranges::sort(indices_sorted, std::less<IndexedVertex>{});

    auto duplicates = std::ranges::unique(indices_sorted);
    return std::vector<IndexedVertex>{indices_sorted.begin(), duplicates.begin()};
}

bool makeVertices(std::vector<IndexedVertex> const &indexed, std::vector<Vec3> const &positions,
                  std::vector<Vec2> const &texture_coords, std::vector<Vertex> &vertices) {
    vertices.clear();
    vertices.reserve(indexed.size());

    for (auto const &[pos_idx, coord_idx] : indexed) {
        if (pos_idx < 1 || pos_idx > std::ssize(positions))
            return false;

        if (coord_idx.has_value() && (*coord_idx < 1 || *coord_idx > std::ssize(texture_coords)))
            return false;

        vertices.push_back(Vertex{.position = positions[pos_idx - 1],
                                  .texture_coords = coord_idx ? texture_coords[*coord_idx - 1] : Vec2{0.5, 0.5}});
    }

    return true;
}

bool meshFromIndexedData(std::vector<Vec3> const &vertices, std::vector<Vec2> const &texture_coords,
                         std::vector<IndexedVertex> const &indexed, Mesh &mesh) {
    auto deduplicated_indices = deduplicateIndexedVertices(indexed);

    auto final_vertices = std::vector<Vertex>{};
    if (!makeVertices(deduplicated_indices, vertices, texture_coords, final_vertices))
        return false;

    auto final_indices = std::vector<int>{};
    final_indices.reserve(indexed.size());
    for (auto const &indexed_vertex : indexed) {
        auto it = std::ranges::lower_bound(deduplicated_indices, indexed_vertex, std::less{});
        assert(it != deduplicated_indices.end());
        assert(*it == indexed_vertex);
        final_indices.push_back(std::distance(deduplicated_indices.begin(), it));
    }

    mesh = Mesh{.vertices = std::move(final_vertices), .indices = std::move(final_indices)};
    return true;
}

} // namespace

std::optional<Mesh> readMeshFromFile(std::string const &path) {
    auto input_file = std::ifstream(path);
    if (!input_file.good()) {
        std::cerr << "Failed to open file '" << trimStr(path) << "'" << std::endl;
        return {};
    }

    auto vertices = std::vector<Vec3>{};
    auto texture_coords = std::vector<Vec2>{};
    auto indexed = std::vector<IndexedVertex>{};

    for (std::string line; std::getline(input_file, line);) {
        auto trimmed = trimStr(line);

        if (trimmed.empty())
            continue;

        tryParseLine(trimmed, vertices, texture_coords, indexed);
    }

    Mesh mesh;
    if (!meshFromIndexedData(vertices, texture_coords, indexed, mesh))
        return {};

    return mesh;
}
