#include "drawing.h"
#include "window.h"

auto main(int argc, char *argv[]) -> int {
    (void)argc;
    (void)argv;

    cv::Mat frameBuffer = cv::Mat::zeros(cv::Size{800, 600}, CV_8UC3);
    auto main_window = Window("Renderer demo");

    while (true) {
        clearImage(frameBuffer, cv::Vec3f(255, 200, 200));

        auto key = main_window.showAndGetKey(frameBuffer);
        if (key == 27) {
            break;
        }
    }
}
