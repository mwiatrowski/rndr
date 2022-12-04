#include "drawing.h"
#include "math.h"
#include "window.h"

namespace {

auto const TRIANGLE = Triangle{Vec2{100, 500}, Vec2{400, 100}, Vec2{700, 500}};

}

auto main(int argc, char *argv[]) -> int {
    (void)argc;
    (void)argv;

    cv::Mat frameBuffer = cv::Mat::zeros(cv::Size{800, 600}, CV_8UC3);
    auto main_window = Window("Renderer demo");

    while (true) {
        clearImage(frameBuffer, cv::Vec3f(255, 200, 200));
        drawTriangle(frameBuffer, TRIANGLE, cv::Vec3f(100, 100, 200));

        auto key = main_window.showAndGetKey(frameBuffer);
        if (key == 27) {
            break;
        }
    }
}
