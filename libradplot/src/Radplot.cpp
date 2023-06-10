// Radplot
// 
// Copyright 2023 Mike Frampton
//

#include "radplot/Radplot.h"

#include <iostream>
#include "Radplot.h"

namespace radplot
{

Figure::Figure() :
    _pdata(nullptr)
{
    std::cout << "Creating a Figure\n";
}

void Figure::Show()
{
    // TODO
}
}