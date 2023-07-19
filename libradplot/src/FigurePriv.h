
#pragma once

#include "radplot/Figure.h"
#include "Renderer.h"

#include <glm/glm.hpp>
#include <thread>

namespace radplot {

class FigurePriv {
public:
    FigurePriv();

    void Show(bool block = true);

private:
    void InitWindow();
    void InitEvents(EventHandler& events);

    void OnMouseClickEvent(MouseClickEvent event);
    void OnMouseMoveEvent(MouseMoveEvent event);
    void OnDragEvent(MouseDragEvent event);
    void OnScrollEvent(MouseScrollEvent event);

    // Log on window or input events.
    static constexpr bool LogEvents = false;

    std::thread _render_thread;
    std::unique_ptr<Renderer> _renderer;

    Camera::ViewState _drag_start;
    float _drag_scale;
};

}  // namespace radplot