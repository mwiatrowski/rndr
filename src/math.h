#pragma once

#include <array>

struct Vec2 {
    float x;
    float y;
};

auto operator-(Vec2 const &lhs, Vec2 const &rhs) -> Vec2;

auto cross(Vec2 const &lhs, Vec2 const &rhs) -> float;

struct Vec3 {
    float x;
    float y;
    float z;
};

auto dot(Vec3 const &lhs, Vec3 const &rhs) -> float;

template <int Rows, int Cols> struct Matrix : public std::array<float, Rows * Cols> {
    float &at(int row, int col) { return std::array<float, Rows * Cols>::at(row * Rows + col); }
    float const &at(int row, int col) const { return std::array<float, Rows * Cols>::at(row * Rows + col); }
};

template <int A, int B, int C> auto operator*(Matrix<A, B> const &lhs, Matrix<B, C> const &rhs) -> Matrix<A, C> {
    auto result = Matrix<A, C>{};

    for (int row = 0; row < A; ++row) {
        for (int col = 0; col < C; ++col) {
            result.at(row, col) = 0;
            for (int i = 0; i < B; ++i) {
                result.at(row, col) += lhs.at(row, i) * rhs.at(i, col);
            }
        }
    }

    return result;
}

using Vec4 = Matrix<1, 4>;
using Mat4 = Matrix<4, 4>;
