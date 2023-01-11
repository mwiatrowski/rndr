#include <cmath>

#include "benchmark.h"
#include "drawing.h"
#include "framebuffer.h"
#include "math.h"
#include "transform.h"
#include "window.h"

namespace {

auto WINDOW_WIDTH = 800;
auto WINDOW_HEIGHT = 600;

auto const QUAD = Mesh{Triangle{Vec3{-0.5, -0.5, 0.2}, Vec3{-0.5, 0.5, -0.2}, Vec3{0.5, -0.5, 0.2}},
                       Triangle{Vec3{0.5, -0.5, 0.2}, Vec3{-0.5, 0.5, -0.2}, Vec3{0.5, 0.5, -0.2}}};

auto POSITIONS = std::array{
    Vec3{-2.0, 2.0, 4.0},
    Vec3{-4.0, -4.0, 8.0},
    Vec3{0, 0, 1.0},
    Vec3{1.0, 0.0, 2.0},
};

} // namespace

auto main(int argc, char *argv[]) -> int {
    (void)argc;
    (void)argv;

    auto frame_buffer = createFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
    auto main_window = Window("Renderer demo");

    auto aspect_ratio = WINDOW_WIDTH / static_cast<float>(WINDOW_HEIGHT);
    auto projection = projectionTransform(70, aspect_ratio);

    auto frame_count = 0;
    auto frame_timer = BenchmarkTimer();

    while (true) {
        clear(frame_buffer, cv::Vec3b(255, 200, 200));

        for (auto const &position : POSITIONS) {
            auto translation = translationTransform(position);
            drawMesh(frame_buffer, QUAD, translation * projection);
        }

        auto key = main_window.showAndGetKey(frame_buffer.render_target);
        if (key == 27) {
            break;
        }

        auto frame_duration_ms = std::round(frame_timer.GetNanosAndReset() * 1.e-6f);
        frame_count += 1;

        std::cout << "Frame " << frame_count << " took " << frame_duration_ms << " ms" << std::endl;
    }
}
