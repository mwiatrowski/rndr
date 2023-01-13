#include "wavefront.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

#include "drawing.h"

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

// Input must be trimmed
std::optional<Vec3> tryParseVertex(std::string const &line) {
    if (!line.starts_with("v"))
        return {};

    std::string prefix;
    float x, y, z;
    auto stream = std::stringstream{line};
    stream >> prefix >> x >> y >> z;

    if (stream.fail() || !stream.eof())
        return {};

    // Flip X and Y axes to match what the rest of the engine expects.
    return {Vec3{-x, -y, z}};
}

// Input must be trimmed
std::optional<Triangle> tryParseFace(std::string const &line, std::vector<Vec3> const &vertices) {
    if (!line.starts_with("f"))
        return {};

    std::string prefix;
    uint32_t v1, v2, v3;
    auto stream = std::stringstream{line};
    stream >> prefix >> v1 >> v2 >> v3;

    if (stream.fail() || !stream.eof())
        return {};

    if (v1 == 0 || v1 > vertices.size() || v2 == 0 || v2 > vertices.size() || v3 == 0 || v3 > vertices.size())
        return {};

    return {Triangle{vertices[v1 - 1], vertices[v2 - 1], vertices[v3 - 1]}};
}

} // namespace

std::optional<Mesh> readMeshFromFile(std::string const &path) {
    auto vertices = std::vector<Vec3>{};
    auto faces = std::vector<Triangle>{};

    auto input_file = std::ifstream(path);
    if (!input_file.good()) {
        std::cerr << "Failed to open file '" << trimStr(path) << "'" << std::endl;
        return {};
    }

    for (std::string line; std::getline(input_file, line);) {
        auto trimmed = trimStr(line);

        if (trimmed.empty())
            continue;

        if (auto vertex = tryParseVertex(trimmed)) {
            vertices.emplace_back(std::move(*vertex));
        } else if (auto face = tryParseFace(trimmed, vertices)) {
            faces.emplace_back(std::move(*face));
        }
    }

    return faces;
}
