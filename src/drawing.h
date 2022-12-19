#pragma once

#include <array>
#include <vector>

#include <opencv2/opencv.hpp>

#include "math.h"

auto clearImage(cv::Mat &img, cv::Vec3b color) -> void;

using Vertex = Vec3;
using Triangle = std::array<Vertex, 3>;
using Mesh = std::vector<Triangle>;

auto drawTriangle(cv::Mat &img, Triangle const &vertices, Mat4 const &transform) -> void;
auto drawMesh(cv::Mat &img, Mesh const &mesh, Mat4 const &transform) -> void;
