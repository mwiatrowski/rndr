#pragma once

#include <chrono>

class BenchmarkTimer {
    std::chrono::time_point<std::chrono::steady_clock> last_reset_;

  public:
    BenchmarkTimer();
    int64_t GetNanosAndReset();
};
