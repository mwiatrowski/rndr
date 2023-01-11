#pragma once

#include <opencv2/opencv.hpp>

struct FrameBuffer {
    cv::Mat render_target;
    cv::Mat depth_buffer;
};

auto createFrameBuffer(int width, int height) -> FrameBuffer;

auto clear(FrameBuffer &fb, cv::Vec3b color) -> void;
auto setPixel(FrameBuffer &fb, int x, int y, float inv_depth, cv::Vec3b color) -> void;
