// Radplot
// 
// Copyright 2023 Mike Frampton
//

#pragma once

namespace radplot
{

class Figure
{
public:
    Figure();

    // Create a window for the Figure and run its event loop.
    void Show();

private:
    struct FigureData;
    FigureData* _pdata;
};

}