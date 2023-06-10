// Radplot
// 
// Copyright 2023 Mike Frampton
//
#include "Radplot.h"

#include <iostream>
#include <format>

LOG_MODULE(LOG_MODULE_FIGURE)

namespace radplot
{

struct Figure::FigureData
{
    Window* _window = nullptr;
};

Figure::Figure() :
    _pdata(nullptr)
{
    LOG_INFO("New Figure");

    _pdata = new FigureData();
}

void Figure::Show()
{
    _pdata->_window = new Window();
    _pdata->_window->RunEventLoop();
}
}