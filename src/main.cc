#include <chrono>
#include <cmath>

#include "benchmark.h"
#include "drawing.h"
#include "framebuffer.h"
#include "math.h"
#include "mesh.h"
#include "transform.h"
#include "wavefront.h"
#include "window.h"

namespace {

auto WINDOW_WIDTH = 1920;
auto WINDOW_HEIGHT = 1080;

auto OBJECT_POSITION = Vec3{123.4352, 432.1235, -543.123};
auto CAMERA_DISTANCE_FACTOR = 150.f;

constexpr char MESH_FILE[] = "../resources/12328_Statue_v1_L2.obj";

auto nowSeconds() -> double {
    using namespace std::chrono;

    auto now = steady_clock::now().time_since_epoch();
    auto as_nanos = duration_cast<nanoseconds>(now);
    return as_nanos.count() * 1.e-9;
}

} // namespace

auto main(int argc, char *argv[]) -> int {
    (void)argc;
    (void)argv;

    auto frame_buffer = createFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
    auto main_window = Window("Renderer demo");

    auto aspect_ratio = WINDOW_WIDTH / static_cast<float>(WINDOW_HEIGHT);
    auto projection = projectionTransform(70, aspect_ratio);

    auto displayed_mesh = readMeshFromFile(MESH_FILE);
    if (!displayed_mesh) {
        std::cerr << "Failed to load the mesh from file!" << std::endl;
        return 1;
    }

    auto frame_count = 0;
    auto frame_timer = BenchmarkTimer();

    while (true) {
        auto time_now = nowSeconds();

        clear(frame_buffer, cv::Vec3b(255, 200, 200));

        auto camera_displacement = Vec3{
            static_cast<float>(std::sin(time_now * 0.25463234 + 2.354313)), //
            static_cast<float>(std::sin(time_now * 0.45231456 + 3.4313)),   //
            static_cast<float>(std::sin(time_now * 0.35132254 + 1.2324544)) //
        };
        auto camera_position = OBJECT_POSITION + CAMERA_DISTANCE_FACTOR * camera_displacement;
        auto camera_transform = lookAt(camera_position, OBJECT_POSITION, Vec3{0.0, 0.0, 1.0});

        auto object_translation = translationTransform(Vec3{0, 0, -100}) * translationTransform(OBJECT_POSITION);
        drawMesh(frame_buffer, *displayed_mesh, object_translation * camera_transform * projection);

        auto key = main_window.showAndGetKey(frame_buffer.render_target);
        if (key == 27) {
            break;
        }

        auto frame_duration_ms = std::round(frame_timer.GetNanosAndReset() * 1.e-6f);
        frame_count += 1;

        std::cout << "Frame " << frame_count << " took " << frame_duration_ms << " ms" << std::endl;
    }
}
