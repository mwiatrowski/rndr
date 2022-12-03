#include "drawing.h"

auto clearImage(cv::Mat &img, cv::Vec3f color) -> void { img.setTo(color); }
