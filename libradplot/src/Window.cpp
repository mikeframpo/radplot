
#include "Radplot.h"

#include <unistd.h>

namespace radplot
{

Window::Window()
{
    // subsequent calls to glfwInit() return GLFW_TRUE immediately.
    // TODO: we should create a one-time init function anyways.
    if (!glfwInit())
        throw new RadException("Failed to initialise GLFW");

    _pwindow = glfwCreateWindow(1024, 768, "Figure X", nullptr, nullptr);
    if (!_pwindow)
        throw RadException("GLFW window creation failed.");}

void Window::RunEventLoop()
{    
    while (!glfwWindowShouldClose(_pwindow))
    {
        glfwPollEvents();
        usleep(1000);
    }
}

}
