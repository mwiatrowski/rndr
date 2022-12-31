#include "drawing.h"
#include "math.h"
#include "transform.h"
#include "window.h"

namespace {

auto WINDOW_WIDTH = 640;
auto WINDOW_HEIGHT = 480;

auto const QUAD = Mesh{Triangle{Vec3{-0.5, -0.5, 0}, Vec3{-0.5, 0.5, 0}, Vec3{0.5, -0.5, 0}},
                       Triangle{Vec3{0.5, -0.5, 0}, Vec3{-0.5, 0.5, 0}, Vec3{0.5, 0.5, 0}}};

auto POSITIONS = std::array{
    Vec3{0, 0, 1.0},
    Vec3{1.0, 0.0, 2.0},
    Vec3{-2.0, 2.0, 4.0},
    Vec3{-4.0, -4.0, 8.0},
};

} // namespace

auto main(int argc, char *argv[]) -> int {
    (void)argc;
    (void)argv;

    cv::Mat frameBuffer = cv::Mat::zeros(cv::Size{WINDOW_WIDTH, WINDOW_HEIGHT}, CV_8UC3);
    auto main_window = Window("Renderer demo");

    auto aspect_ratio = WINDOW_WIDTH / static_cast<float>(WINDOW_HEIGHT);
    auto projection = projectionTransform(70, aspect_ratio, 0.01, 123.0);

    while (true) {
        clearImage(frameBuffer, cv::Vec3f(255, 200, 200));

        for (auto const &position : POSITIONS) {
            auto translation = translationTransform(position);
            drawMesh(frameBuffer, QUAD, translation * projection);
        }

        auto key = main_window.showAndGetKey(frameBuffer);
        if (key == 27) {
            break;
        }

        static auto frame_count = 0;
        std::cout << "FRAME " << frame_count++ << std::endl;
    }
}
