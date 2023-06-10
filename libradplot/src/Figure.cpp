// Radplot
// 
// Copyright 2023 Mike Frampton
//
#define LOG_MODULE "Figure"
#include "Log.h"

#include "Radplot.h"

#include <iostream>
#include <format>

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