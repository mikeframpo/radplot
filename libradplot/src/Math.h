// Radplot
//
// Copyright 2023 Mike Frampton
//
#pragma once

namespace radplot {

struct Vec3 {
    float x;
    float y;
    float z;

    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
};

}  // namespace radplot