
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

Window::Window() : _pwindow(nullptr), _events(nullptr), _win_size(1024, 768) {
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

    _pwindow = glfwCreateWindow(_win_size.x, _win_size.y, "radplot", nullptr, nullptr);
    if (!_pwindow)
        throw RadException("GLFW window creation failed.");

    // Set event handler private data ptr to *this
    glfwSetWindowUserPointer(_pwindow, this);

    glfwGetWindowSize(_pwindow, &_win_size.x, &_win_size.y);
    glfwMakeContextCurrent(_pwindow);

    static auto winResizeCB = [](GLFWwindow* window, int width, int height) {
        Window* win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        win->_win_size.x = width;
        win->_win_size.y = height;
    };
    glfwSetWindowSizeCallback(_pwindow, winResizeCB);

    Window::InitGL();
}

Window::~Window() {
    glfwDestroyWindow(_pwindow);
}

glm::ivec2 Window::GetSize() const {
    return _win_size;
}

void Window::RunEventLoop(RenderFunc doRender, EventHandler* event_handler) {
    LOG_INFO("RunEventLoop");

    DEBUG_ASSERT(event_handler != nullptr);
    AttachEvents(event_handler);

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

void Window::AttachEvents(EventHandler* handler) {
    _events = handler;

    static auto on_mousebutton_cb = [](GLFWwindow* window, int button, int action, int mods) {
        Window* win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        EventHandler* handler = win->_events;

        // if pressed and no drag handler already, create one
        // else if released and drag handler matches released, reset drag handler
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        int ixpos = (int)xpos;
        int iypos = (int)ypos;

        MouseButtons rbutton = MouseButtons::None;
        if (button == GLFW_MOUSE_BUTTON_1)
            rbutton = MouseButtons::Left;
        else if (button == GLFW_MOUSE_BUTTON_2)
            rbutton = MouseButtons::Right;
        else if (button == GLFW_MOUSE_BUTTON_3)
            rbutton = MouseButtons::Middle;

        if (handler->OnMouseClick)
            handler->OnMouseClick({ixpos, iypos, action == GLFW_PRESS, rbutton});

        if (action == GLFW_PRESS) {
            if (!handler->_drag) {
                // button pressed, create a new drag event
                handler->_drag = MouseDragEvent{ixpos, iypos, ixpos, iypos, true, rbutton};

                if (handler->OnMouseDrag != nullptr)
                    handler->OnMouseDrag(*handler->_drag);
            }
        } else if (action == GLFW_RELEASE) {
            if (handler->_drag && rbutton == handler->_drag->Button) {
                // drag button released, end drag
                handler->_drag.reset();
            }
        }
    };
    glfwSetMouseButtonCallback(_pwindow, on_mousebutton_cb);

    static auto on_cursor_cb = [](GLFWwindow* window, double xpos, double ypos) {
        // get private data ptr.
        Window* win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        EventHandler* handler = win->_events;

        auto buttons = MouseButtons::None;
        auto set_button = [&](int glfw_button, MouseButtons button) {
            buttons |= (glfwGetMouseButton(window, glfw_button) == GLFW_PRESS) ? button : MouseButtons::None;
        };

        set_button(GLFW_MOUSE_BUTTON_1, MouseButtons::Left);
        set_button(GLFW_MOUSE_BUTTON_2, MouseButtons::Right);
        set_button(GLFW_MOUSE_BUTTON_3, MouseButtons::Middle);

        int ixpos = (int)xpos;
        int iypos = (int)ypos;

        int xrel = 0;
        int yrel = 0;
        if (handler->_x_mouse != -1 && handler->_y_mouse != -1) {
            xrel = ixpos - handler->_x_mouse;
            yrel = iypos - handler->_y_mouse;
        }
        handler->_x_mouse = ixpos;
        handler->_y_mouse = iypos;

        MouseMoveEvent event{ixpos, iypos, xrel, yrel, buttons};
        if (handler->OnMouseMove != nullptr)
            handler->OnMouseMove(event);

        if (handler->_drag && handler->OnMouseDrag != nullptr) {
            handler->_drag->XPos = ixpos;
            handler->_drag->YPos = iypos;
            handler->_drag->IsDragStart = false;
            handler->OnMouseDrag(*handler->_drag);
        }
    };
    glfwSetCursorPosCallback(_pwindow, on_cursor_cb);

    // TODO: reset xprev on enter window (callback)

    static auto on_scroll_cb = [](GLFWwindow* window, double xoffset, double yoffset) {
        Window* win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        EventHandler* handler = win->_events;

        if (handler->OnMouseScroll){
            handler->OnMouseScroll({static_cast<float>(yoffset)});
        }
    };
    glfwSetScrollCallback(_pwindow, on_scroll_cb);
}

}  // namespace radplot
