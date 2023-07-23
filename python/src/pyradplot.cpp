
#include "pyradplot.h"

#include "radplot/Figure.h"

#include <cstdio>

//https://asiffer.github.io/posts/numpy/
/*
void PrintArray(double *v, size_t n) {
    for (size_t i = 0; i < n; i++) {
        printf("%f ", v[i]);
    }
    printf("\n");
}
*/

Figure *CreateFigure() {
    radplot::Figure* figure = new radplot::Figure();
    figure->Show(false);

    return (Figure*)figure;
}

void FigureWaitForClose(struct Figure* pfigure) {
    radplot::Figure* figure = (radplot::Figure*)pfigure;
    figure->WaitForClose();
}

void FigureDrawQuad(struct Figure* pfigure, PoseArgs pose) {
    radplot::Figure* figure = (radplot::Figure*)pfigure;
    
    radplot::Vec3 pos(pose.Location[0], pose.Location[1], pose.Location[2]);
    figure->DrawQuad(pos);
}
