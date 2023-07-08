#pragma once

#include <GLFW/glfw3.h>
#include <functional>
#include <optional>

namespace radplot {

enum class MouseButtons {
    None = 0,
    Left = (1 << 1),
    Right = (1 << 2),
    Middle = (1 << 3),
};

inline constexpr MouseButtons& operator|=(MouseButtons& x, MouseButtons y) {
    x = static_cast<MouseButtons>(static_cast<int>(x) | static_cast<int>(y));
    return x;
}

struct MouseClickEvent {
    int XPos;
    int YPos;
    MouseButtons Button;
};

struct MouseMoveEvent {
    int XPos;
    int YPos;
    int XRel;
    int YRel;
    MouseButtons Buttons;
};

struct MouseDragEvent {
    int XPos;
    int YPos;
    int XStart;
    int YStart;
    bool IsDragStart;
    MouseButtons Button;
};

struct EventHandler {
    using MouseClickCallback = std::function<void(MouseClickEvent)>;
    using MouseMoveCallback = std::function<void(MouseMoveEvent)>;
    using MouseDragCallback = std::function<void(MouseDragEvent)>;

    MouseClickCallback OnMouseClick = nullptr;
    MouseMoveCallback OnMouseMove = nullptr;
    MouseDragCallback OnMouseDrag = nullptr;

    int _x_mouse;
    int _y_mouse;
    std::optional<MouseDragEvent> _drag;
};

class Window {
public:
    using RenderFunc = std::function<void(void)>;

    // Creates a Window and displays it immediately.
    Window();
    void GetSize(int* xsize, int* ysize) const;

    void RunEventLoop(RenderFunc doRender, EventHandler& event_handler);

private:
    static void InitGL();

    void AttachEvents(EventHandler& handler);

    GLFWwindow* _pwindow;
};

}  // namespace radplot