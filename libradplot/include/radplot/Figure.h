// Radplot
//
// Copyright 2023 Mike Frampton
//
#pragma once

#include "Math.h"
#include <memory>

namespace radplot {

class Figure {
public:
    Figure();
    ~Figure();

    // Create a window for the Figure and run its event loop.
    void Show(bool block = true);
    void WaitForClose();

    // TODO: maybe this should return a non-const entity, so that it can be modified in place
    void DrawCube(Vec3 centre);
    void DrawQuad(Vec3 centre);

private:
    struct FigureData;
    std::unique_ptr<FigureData> _pdata;
};

}  // namespace radplot