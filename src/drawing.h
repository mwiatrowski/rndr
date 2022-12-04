#pragma once

#include <array>

#include <opencv2/opencv.hpp>

auto clearImage(cv::Mat &img, cv::Vec3b color) -> void;

using Vertex = cv::Vec2f;
using Triangle = std::array<Vertex, 3>;

auto drawTriangle(cv::Mat &img, Triangle const &vertices, cv::Vec3b color) -> void;
