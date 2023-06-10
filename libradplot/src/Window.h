#pragma once

#include <GLFW/glfw3.h>

namespace radplot
{

class Window
{
public:
    Window();
    void RunEventLoop();

private:
    GLFWwindow* _pwindow;
};

}