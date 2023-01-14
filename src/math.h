#pragma once

#include <array>
#include <ostream>

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

auto operator<<(std::ostream &stream, Vec3 const &vec) -> std::ostream &;

auto operator+(Vec3 const &lhs, Vec3 const &rhs) -> Vec3;
auto operator-(Vec3 const &vec) -> Vec3;
auto operator-(Vec3 const &lhs, Vec3 const &rhs) -> Vec3;
auto operator*(float scalar, Vec3 const &vec) -> Vec3;

auto dot(Vec3 const &lhs, Vec3 const &rhs) -> float;
auto cross(Vec3 const &lhs, Vec3 const &rhs) -> Vec3;

auto norm(Vec3 const &vec) -> float;
auto normalize(Vec3 const &vec) -> Vec3;

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

template <int A, int B> auto operator<<(std::ostream &stream, Matrix<A, B> const &mat) -> std::ostream & {
    stream << "{ ";
    for (int row = 0; row < A; ++row) {
        stream << "{ ";
        for (int col = 0; col < B; ++col) {
            stream << mat.at(row, col) << " ";
        }
        stream << "} ";
    }
    stream << "}";
    return stream;
}

using Vec4 = Matrix<1, 4>;
using Mat4 = Matrix<4, 4>;
