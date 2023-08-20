// Radplot
//
// Copyright 2023 Mike Frampton
//
#pragma once

extern "C" {
    // Opaque structure to cpp Figure
    struct Figure;

    typedef double* Vec3Ptr;

    // Arguments for drawing a generic geometry
    struct PoseArgs {
        // Object central location
        Vec3Ptr Location;

        // TODO: add other Model matrix parameters
    };

    Figure* CreateFigure();

    void FigureWaitForClose(struct Figure* figure);

    void FigureDrawQuad(struct Figure* figure, struct PoseArgs pose);
}