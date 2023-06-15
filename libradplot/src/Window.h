#pragma once

#include <GLFW/glfw3.h>
#include <functional>

namespace radplot {

class Window {
public:
    using RenderFunc = std::function<void(void)>;

    // Creates a Window and displays it immediately.
    Window();

    void RunEventLoop(RenderFunc doRender);

private:
    static void InitGL();

    GLFWwindow* _pwindow;
};

}  // namespace radplot