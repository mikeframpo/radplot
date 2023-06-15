// Radplot
//
// Copyright 2023 Mike Frampton
//
#pragma once

#include "Math.h"

namespace radplot {

class Figure {
public:
    Figure();

    // Create a window for the Figure and run its event loop.
    void Show(bool block = true);

    // TODO: maybe this should return a non-const entity, so that it can be modified in place
    void DrawCube(Vec3 centre);

private:
    struct FigureData;
    FigureData* _pdata;
};

}  // namespace radplot