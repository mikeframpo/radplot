
#include "radplot/Figure.h"

#include <iostream>
#include <thread>
#include <unistd.h>

int main(int argc, char** argv) {
    radplot::Figure figure;
    figure.Show();

    figure.DrawCube({0, 0, 0});
}
