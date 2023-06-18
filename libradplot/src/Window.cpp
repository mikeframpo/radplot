
// OpenGL loader library must be included before GLFW so that it's aware of the bindings.
#include <GL/glew.h>
#include "Window.h"

#include "Radplot.h"

#include <GLFW/glfw3.h>
#include <unistd.h>

#include <format>
#include <atomic>

namespace radplot {

LOG_MODULE(LogModule::Window);

void GLFWErrorHandler(int errorCode, const char* description) {
    LOG_ERROR("GLFW error 0x%x, %s\n", errorCode, description);
}

void GLAPIENTRY GLErrorHandler(GLenum source,
                               GLenum type,
                               GLuint id,
                               GLenum severity,
                               GLsizei length,
                               const GLchar* message,
                               const void* userParam) {
    if (type == GL_DEBUG_TYPE_ERROR) {
        fprintf(stderr, "GL Error: source: 0x%x, type = 0x%x, severity = 0x%x\n\tmessage = %s\n", source, type,
                severity, message);

#ifdef EXIT_ON_GL_ERROR
        throw PP3Exception("PP3 GL Error");
#endif
    }
}

Window::Window() : _pwindow(nullptr) {
    LOG_INFO("New Window");

    // TODO: All this stuff can only be called on the main thread, for now only need to support 1 window. Peehaps we
    //  can create Window on main thread and then move context to background thread.

    // Additional calls to glfwInit return immediately.
    if (!glfwInit())
        throw RadException("Failed to initialise GLFW");

    glfwSetErrorCallback(GLFWErrorHandler);
    // Set the window hints.

    // Minimum GL version 4.3 as we're using DebugMessageCallback
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _pwindow = glfwCreateWindow(1024, 768, "radplot", nullptr, nullptr);
    if (!_pwindow)
        throw RadException("GLFW window creation failed.");

    glfwMakeContextCurrent(_pwindow);

    Window::InitGL();
}

void Window::RunEventLoop(RenderFunc doRender) {
    LOG_INFO("RunEventLoop");

    // Note: multiple glfw windows must be created on the main thread, so if we want to support that then we need
    // an 'event' thread or something, in addition to render threads.

    while (!glfwWindowShouldClose(_pwindow)) {

        // TODO: better framerate control/measurement
        doRender();
        glfwSwapBuffers(_pwindow);
        glfwPollEvents();

        usleep(1000);
    }
}

void Window::InitGL() {
    static std::atomic<bool> gl_initialised = ATOMIC_FLAG_INIT;
    if (!gl_initialised.exchange(true)) {

        GLenum err = glewInit();
        if (err != GLEW_OK)
            throw RadException(std::format("GLEW Init failed. Err: {}", err));

        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(GLErrorHandler, 0);
        glDebugMessageControl(GL_DEBUG_SOURCE_OTHER, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_FALSE);

        LOG_INFO("OpenGL version: %s", glGetString(GL_VERSION));
    }
}

}  // namespace radplot
