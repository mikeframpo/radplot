// Radplot
// 
// Copyright 2023 Mike Frampton
//
#include "Radplot.h"

#include <iostream>
#include <format>

namespace radplot
{

LOG_MODULE(LogModule::Figure);

struct Figure::FigureData
{
    Window* _window = nullptr;
};

Figure::Figure() :
    _pdata(nullptr)
{
    // TODO: move this to an API or defines
    LogSetLevel(LogLevel::Info);
    LogEnableAllModules();

    LOG_INFO("New Figure");

    _pdata = new FigureData();
}

void Figure::Show()
{
    _pdata->_window = new Window();
    _pdata->_window->RunEventLoop();
}
}