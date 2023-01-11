#include "framebuffer.h"

#include <opencv2/opencv.hpp>

auto createFrameBuffer(int width, int height) -> FrameBuffer {
    auto render_target = cv::Mat::zeros(cv::Size{width, height}, CV_8UC3);
    auto depth_buffer = cv::Mat::zeros(cv::Size{width, height}, CV_32FC1);

    return FrameBuffer{.render_target = std::move(render_target), .depth_buffer = std::move(depth_buffer)};
}

auto clear(FrameBuffer &fb, cv::Vec3b color) -> void {
    fb.render_target.setTo(color);
    fb.depth_buffer.setTo(0.0);
}

auto setPixel(FrameBuffer &fb, int x, int y, float inv_depth, cv::Vec3b color) -> void {
    if (inv_depth < 0.0) {
        return;
    }

    if (fb.depth_buffer.at<float>(y, x) >= inv_depth) {
        return;
    }

    fb.depth_buffer.at<float>(y, x) = inv_depth;
    fb.render_target.at<cv::Vec3b>(y, x) = color;
}
