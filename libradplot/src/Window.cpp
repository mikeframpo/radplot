
// OpenGL loader library must be included before GLFW so that it's aware of the bindings.
// #include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Radplot.h"

#include <unistd.h>
#include <format>
#include "Window.h"

namespace radplot
{

LOG_MODULE(LogModule::Window);

void GLContextInit()
{
    // TODO: one-time lock

    // subsequent calls to glfwInit() return GLFW_TRUE immediately.
    if (!glfwInit())
        throw RadException("Failed to initialise GLFW");

    // Set the window hints.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // GLenum err = glewInit();
    // if (err != GLEW_OK)
    //     throw RadException(std::format("GLEW Init failed. Err: {}", err));
}

void GLFWErrorHandler(int errorCode, const char* description)
{
    LOG_ERROR("GLFW error 0x%x, %s\n", errorCode, description);
}

Window::Window() :
    _pwindow(nullptr)
{
    if (!glfwInit())
        throw RadException("Failed to initialise GLFW");

    glfwSetErrorCallback(GLFWErrorHandler);
    // Set the window hints.
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // GLenum err = glewInit();
    // if (err != GLEW_OK)
    //     throw RadException(std::format("GLEW Init failed. Err: {}", err));

    _pwindow = glfwCreateWindow(1024, 768, "Figure X", nullptr, nullptr);
    if (!_pwindow)
        throw RadException("GLFW window creation failed.");
}

void Window::RunEventLoop()
{    
    while (!glfwWindowShouldClose(_pwindow))
    {
        glfwPollEvents();
        usleep(1000);
    }
}

}
