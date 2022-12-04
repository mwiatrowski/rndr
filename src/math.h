#pragma once

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
