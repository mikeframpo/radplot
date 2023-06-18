
#include "radplot/Figure.h"
#include "Radplot.h"

#include <format>
#include <iostream>
#include <thread>
#include <memory>

namespace radplot {

LOG_MODULE(LogModule::Figure);

struct Figure::FigureData {
    std::thread _render_thread;
};

Figure::Figure() : _pdata(std::make_unique<Figure::FigureData>()) {
    if (!_pdata)
        throw RadException("Failed to alloc figure data.");

    // TODO: move this to an API or defines
    LogSetLevel(LogLevel::Trace);
    LogEnableAllModules();

    LOG_INFO("New Figure");
}

Figure::~Figure() {
    // Required for forward declaration.
}

void Figure::Show(bool block) {
    EventHandler events{[&](MouseMoveEvent e) {
        LOG_DEBUG("X %d, Y %d", e.XPos, e.YPos);
    }};

    // Start up the window on a background thread
    _pdata->_render_thread = std::thread([&]() {
        auto window = std::make_unique<Window>();

        Renderer renderer;
        renderer.DrawQuad({1.0, 1.0});

        // TODO: pass an EventHandler to the event loop.
        window->RunEventLoop([&]() { renderer.RenderScene(); }, events);
    });

    if (block)
        _pdata->_render_thread.join();
}

}  // namespace radplot