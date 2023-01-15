cmake_minimum_required(VERSION 3.1)
project (rndr CXX)

set(CMAKE_CXX_STANDARD 23)

add_compile_options(-g)
# add_compile_options(-O3)

add_compile_options(-Wall -Wextra -pedantic -Werror)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)
add_compile_options(-fsanitize=undefined)
add_link_options(-fsanitize=undefined)

set(RENDERER_EXECUTABLE "rndr")
add_executable(${RENDERER_EXECUTABLE}
    src/benchmark.cc
    src/drawing.cc
    src/framebuffer.cc
    src/main.cc
    src/math.cc
    src/mesh.cc
    src/transform.cc
    src/wavefront.cc
    src/window.cc
)

find_package(OpenCV 4.5 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(${RENDERER_EXECUTABLE} ${OpenCV_LIBS})
