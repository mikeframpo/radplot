#pragma once

#include <GLFW/glfw3.h>
#include <functional>

namespace radplot {

enum class MouseButtons {
    None = 0,
    Left = (1 << 1),
    Right = (1 << 2),
};

struct MouseMoveEvent {
    int XPos;
    int YPos;
    MouseButtons Buttons;
};

struct EventHandler {
    using MouseMoveCallback = std::function<void(MouseMoveEvent)>;

    MouseMoveCallback OnMouseMove;
};

class Window {
public:
    using RenderFunc = std::function<void(void)>;

    // Creates a Window and displays it immediately.
    Window();

    void RunEventLoop(RenderFunc doRender, EventHandler& event_handler);

private:
    static void InitGL();

    void AttachEvents(EventHandler& handler);

    GLFWwindow* _pwindow;
};

}  // namespace radplot