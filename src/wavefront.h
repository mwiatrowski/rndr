#pragma once

#include <optional>
#include <string>

#include "mesh.h"

std::optional<Mesh> readMeshFromFile(std::string const &path);
