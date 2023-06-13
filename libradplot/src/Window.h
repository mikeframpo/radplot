#pragma once

#include <GLFW/glfw3.h>

namespace radplot
{

class Window
{
public:
    // Creates a Window and displays it immediately.
    Window();

    void RunEventLoop();

private:
    GLFWwindow* _pwindow;
};

}