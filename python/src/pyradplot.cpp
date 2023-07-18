
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

void *CreateFigure() {
    radplot::Figure* figure = new radplot::Figure();
    figure->Show(false);

    return figure;
}