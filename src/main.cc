#include <cmath>

#include "benchmark.h"
#include "drawing.h"
#include "framebuffer.h"
#include "math.h"
#include "transform.h"
#include "wavefront.h"
#include "window.h"

namespace {

auto WINDOW_WIDTH = 1920;
auto WINDOW_HEIGHT = 1080;

auto POSITIONS = std::array{Vec3{0.0, 0.0, 7.0}};

constexpr char MESH_FILE[] = "../resources/cow-nonormals.obj";

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

    auto displayed_mesh = readMeshFromFile(MESH_FILE);
    if (!displayed_mesh) {
        std::cerr << "Failed to load the mesh from file!" << std::endl;
        return 1;
    }

    while (true) {
        clear(frame_buffer, cv::Vec3b(255, 200, 200));

        for (auto const &position : POSITIONS) {
            auto translation = translationTransform(position);
            drawMesh(frame_buffer, *displayed_mesh, translation * projection);
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
