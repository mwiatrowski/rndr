#include "drawing.h"

#include <algorithm>

namespace {

auto getTriangleBounds(Triangle const &triangle) -> cv::Rect {
    auto const &a = triangle[0];
    auto const &b = triangle[1];
    auto const &c = triangle[2];

    auto x1 = std::min(a[0], std::min(b[0], c[0]));
    auto x2 = std::max(a[0], std::max(b[0], c[0]));
    auto y1 = std::min(a[1], std::min(b[1], c[1]));
    auto y2 = std::max(a[1], std::max(b[1], c[1]));

    return cv::Rect(x1, y1, x2 - x1, y2 - y1);
}

} // namespace

auto clearImage(cv::Mat &img, cv::Vec3b color) -> void { img.setTo(color); }

auto drawTriangle(cv::Mat &img, Triangle const &vertices, cv::Vec3b color) -> void {
    auto bounds = getTriangleBounds(vertices);

    for (int row = bounds.y; row <= bounds.y + bounds.height; ++row) {
        for (int col = bounds.x; col <= bounds.x + bounds.width; ++col) {
            img.at<cv::Vec3b>(row, col) = color;
        }
    }
}
