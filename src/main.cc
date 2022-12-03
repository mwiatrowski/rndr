#include "window.h"

auto main(int argc, char *argv[]) -> int {
    (void)argc;
    (void)argv;

    cv::Mat renderTarget = cv::Mat::zeros(cv::Size{800, 600}, CV_8UC3);

    auto main_window = Window("Renderer demo");
    while (main_window.showAndGetKey(renderTarget) != 27) {
    }
}
