// Radplot
//
// Copyright 2023 Mike Frampton
//
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
    LogSetLevel(LogLevel::Info);
    LogEnableAllModules();

    LOG_INFO("New Figure");

    _pdata = new FigureData();
}

void Figure::Show(bool block) {
    // Start up the window on a background thread
    _pdata->_render_thread = std::thread([]() {
        Renderer renderer;
        auto window = std::make_unique<Window>();

        // TODO: pass an EventHandler to the event loop.
        window->RunEventLoop([&]() { renderer.Render(); });
    });

    if (block)
        _pdata->_render_thread.join();
}

void Figure::DrawCube(Vec3 centre) {
    // TODO
    // Create some draw data:
    //  VertexBuffer
    //  IndexBuffer
    //  Shaders
    //  Uniforms
    // Pass data to the Renderer somehow - command queue? call directly with locking

    // I think that many objects could be drawn with the same buffers but different uniforms e.g. drawing
    // many points at different locations (transform in uniform). So we might want to cache buffers.
}

}  // namespace radplot