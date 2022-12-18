#include "drawing.h"
#include "math.h"
#include "window.h"

namespace {

auto const QUAD = Mesh{Triangle{Vec2{-0.5, -0.5}, Vec2{-0.5, 0.5}, Vec2{0.5, -0.5}},
                       Triangle{Vec2{0.5, -0.5}, Vec2{-0.5, 0.5}, Vec2{0.5, 0.5}}};

} // namespace

auto main(int argc, char *argv[]) -> int {
    (void)argc;
    (void)argv;

    cv::Mat frameBuffer = cv::Mat::zeros(cv::Size{1920, 1080}, CV_8UC3);
    auto main_window = Window("Renderer demo");

    while (true) {
        clearImage(frameBuffer, cv::Vec3f(255, 200, 200));
        drawMesh(frameBuffer, QUAD);

        auto key = main_window.showAndGetKey(frameBuffer);
        if (key == 27) {
            break;
        }

        static auto frame_count = 0;
        std::cout << "FRAME " << frame_count++ << std::endl;
    }
}
