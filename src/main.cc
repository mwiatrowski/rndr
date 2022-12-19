#include "drawing.h"
#include "math.h"
#include "transform.h"
#include "window.h"

namespace {

auto const QUAD = Mesh{Triangle{Vec3{-0.5, -0.5, 0}, Vec3{-0.5, 0.5, 0}, Vec3{0.5, -0.5, 0}},
                       Triangle{Vec3{0.5, -0.5, 0}, Vec3{-0.5, 0.5, 0}, Vec3{0.5, 0.5, 0}}};

} // namespace

auto main(int argc, char *argv[]) -> int {
    (void)argc;
    (void)argv;

    cv::Mat frameBuffer = cv::Mat::zeros(cv::Size{640, 480}, CV_8UC3);
    auto main_window = Window("Renderer demo");

    while (true) {
        clearImage(frameBuffer, cv::Vec3f(255, 200, 200));

        for (int i = 0; i < 3; ++i) {
            auto step = i / 3.f;
            auto translation = translationTransform(Vec3{step, -step, step});
            drawMesh(frameBuffer, QUAD, translation);
        }

        auto key = main_window.showAndGetKey(frameBuffer);
        if (key == 27) {
            break;
        }

        static auto frame_count = 0;
        std::cout << "FRAME " << frame_count++ << std::endl;
    }
}
