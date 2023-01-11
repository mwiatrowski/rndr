#include "benchmark.h"

#include <chrono>

namespace {

template <typename Rep, typename Period> auto toNanoseconds(std::chrono::duration<Rep, Period> const &duration) {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
}

} // namespace

BenchmarkTimer::BenchmarkTimer() : last_reset_{std::chrono::steady_clock::now()} {};

int64_t BenchmarkTimer::GetNanosAndReset() {
    auto now = std::chrono::steady_clock::now();
    auto measurement = now - last_reset_;
    last_reset_ = now;
    return toNanoseconds(measurement);
}
