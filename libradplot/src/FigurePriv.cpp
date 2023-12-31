
#include "FigurePriv.h"

#include <thread>

#include "Radplot.h"

LOG_MODULE(radplot::LogModule::Figure);

namespace radplot {

FigurePriv::FigurePriv() : _render_thread(), _renderer(), _drag_start(), _drag_scale(1.0) {
    // TODO: move this to an API or defines
    LogSetLevel(LogLevel::Trace);
    LogEnableAllModules();

    LOG_INFO("New Figure");
}

void FigurePriv::Show(bool block) {
    // Start up the window on a background thread
    _render_thread = std::thread([&]() {
        InitWindow();
    });

    if (block) {
        WaitForClose();
    }
}

Renderer* FigurePriv::GetRenderer() const {
    return _renderer.get();
}

void FigurePriv::WaitForClose() {
    _render_thread.join();
    LOG_INFO("Figure thread completed");
}

void FigurePriv::InitWindow() {
    // Window as an automatic
    Window window;
    _renderer = std::make_unique<Renderer>(&window);

    EventHandler events;
    InitEvents(events);

    _renderer->DrawGrid();

    // PENDING: move geometry to API
    // _renderer->DrawQuad({1.5, 1.5});
    // _renderer->DrawCube();

    window.RunEventLoop(
        [&]() {
            _renderer->RenderScene();
        },
        &events);

    _renderer = nullptr;
    LOG_INFO("Window closing");
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
    events.OnMouseScroll = [&](MouseScrollEvent e) {
        OnScrollEvent(e);
    };
}

void FigurePriv::OnMouseClickEvent(MouseClickEvent e) {
    LOG_TRACE_IF(LogEvents, "MouseClick: X %d Y %d B %d", e.XPos, e.YPos, e.Button);

    auto& camera = _renderer->GetCamera();
    auto world = camera.WindowToWorld(e.XPos, e.YPos);

    if (e.Press)
        LOG_DEBUG("World: (%0.3f, %0.3f, %0.3f)", world.x, world.y, world.z);
}

void FigurePriv::OnMouseMoveEvent(MouseMoveEvent e) {
    LOG_TRACE_IF(LogEvents, "MouseMove: X %d Y %d B %d", e.XPos, e.YPos);
    // Implement me
}

void FigurePriv::OnDragEvent(MouseDragEvent e) {
    LOG_TRACE_IF(LogEvents, "MouseDrag Member: X %d, Y %d, Xst %d Yst %d B %d", e.XPos, e.YPos, e.XStart, e.YStart,
                 e.Button);
    auto& camera = _renderer->GetCamera();

    if (e.IsDragStart) {
        glm::vec4 click_world = camera.WindowToWorld(e.XStart, e.YStart);
        // Save camera state on drag start.
        _drag_start = camera.GetState();
        _drag_scale = click_world.w;

    } else {
        glm::ivec2 size = camera.GetWindowSize();

        float xmovn = (float)(e.XPos - e.XStart) / size.x;
        float ymovn = (float)(e.YPos - e.YStart) / size.y;

        if (e.Button == MouseButtons::Left) {
            // Left click, pan

            Camera::ViewState translated = _drag_start;

            translated.Translate(xmovn * _drag_scale, ymovn * _drag_scale);
            camera.SetState(translated);

        } else {
            // Right click, rotate around the centre

            constexpr float rads_per_screen = 5.0;
            float xangle = xmovn * rads_per_screen;
            float yangle = ymovn * rads_per_screen;

            // copy state at start of drag
            Camera::ViewState rotated = _drag_start;

            rotated.Rotate(-xangle, -yangle);
            camera.SetState(rotated);
        }
    }
}

void FigurePriv::OnScrollEvent(MouseScrollEvent e) {
    LOG_TRACE_IF(LogEvents, "Scroll %f", e.Scroll);

    auto& camera = _renderer->GetCamera();

    Camera::ViewState zoomed = camera.GetState();

    zoomed.Zoom(-e.Scroll);
    camera.SetState(zoomed);
}

}  // namespace radplot
