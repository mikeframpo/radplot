
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

Figure::Figure() : _pdata(nullptr) {
    // TODO: move this to an API or defines
    LogSetLevel(LogLevel::Trace);
    LogEnableAllModules();

    LOG_INFO("New Figure");

    _pdata = new FigureData();
}

void Figure::Show(bool block) {
    // Start up the window on a background thread
    _pdata->_render_thread = std::thread([]() {
        auto window = std::make_unique<Window>();

        Renderer renderer;
        renderer.DrawQuad({0.5, 0.5});

        // TODO: pass an EventHandler to the event loop.
        window->RunEventLoop([&]() { renderer.RenderScene(); });
    });

    if (block)
        _pdata->_render_thread.join();
}

}  // namespace radplot