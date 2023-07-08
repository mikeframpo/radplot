
#include "FigurePriv.h"
#include "Radplot.h"

#include <thread>

LOG_MODULE(radplot::LogModule::Figure);

namespace radplot {

FigurePriv::FigurePriv() : _render_thread(), _renderer(), _drag_start(), _drag_scale(1.0) {
    // TODO: move this to an API or defines
    LogSetLevel(LogLevel::Debug);
    LogEnableAllModules();

    LOG_INFO("New Figure");
}

void FigurePriv::Show(bool block) {
    // Start up the window on a background thread
    _render_thread = std::thread([&]() {
        InitWindow();
    });

    if (block) {
        _render_thread.join();
        LOG_INFO("Figure thread completed");
    }
}

void FigurePriv::InitWindow() {
    _window = std::make_unique<Window>();
    _renderer = std::make_unique<Renderer>(_window.get());

    EventHandler events;
    InitEvents(events);

    // PENDING: move geometry to API
    _renderer->DrawQuad({0.5, 0.5});

    _window->RunEventLoop(
        [&]() {
            _renderer->RenderScene();
        },
        &events);
}

void radplot::FigurePriv::InitEvents(EventHandler& events) {
    events.OnMouseClick = [&](MouseClickEvent e) {
        OnMouseClickEvent(e);
    };
    events.OnMouseMove = [&](MouseMoveEvent e) {
        OnMouseMoveEvent(e);
    };
    events.OnMouseDrag = [&](MouseDragEvent e) {
        OnDragEvent(e);
    };
}

void FigurePriv::OnMouseClickEvent(MouseClickEvent e) {
    LOG_TRACE("MouseClick: X %d Y %d B %d", e.XPos, e.YPos, e.Button);

    auto& camera = _renderer->GetCamera();
    auto world = camera.WindowToWorld(e.XPos, e.YPos);

    if (e.Press)
        LOG_DEBUG("World: (%0.3f, %0.3f, %0.3f)", world.x, world.y, world.z);
}

void FigurePriv::OnMouseMoveEvent(MouseMoveEvent e) {
    LOG_TRACE("MouseMove: X %d Y %d B %d", e.XPos, e.YPos);
    // Implement me
}

void FigurePriv::OnDragEvent(MouseDragEvent e) {
    LOG_TRACE("MouseDrag Member: X %d, Y %d, Xst %d Yst %d B %d", e.XPos, e.YPos, e.XStart, e.YStart, e.Button);
    auto& camera = _renderer->GetCamera();

    if (e.IsDragStart) {
        glm::vec4 click_world = camera.WindowToWorld(e.XStart, e.YStart);
        _drag_start = camera.GetPosition();
        _drag_scale = click_world.w;
    } else {
        glm::ivec2 size = _window->GetSize();

        float xmovn = (float)(e.XPos - e.XStart) / size.x;
        float ymovn = (float)(e.YPos - e.YStart) / size.y;

        // TODO: also need to move in the camera look-up plane
        glm::vec3 newPos = _drag_start - (glm::vec3(xmovn * _drag_scale, -ymovn * _drag_scale, 0.0f));
        camera.SetPosition(newPos);
    }
}

}  // namespace radplot
