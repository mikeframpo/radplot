
#include "radplot/Figure.h"
#include "../Radplot.h"

namespace radplot {

struct Figure::FigureData {
    FigurePriv _figure;
};

Figure::Figure() : _pdata(std::make_unique<Figure::FigureData>()) {
    if (!_pdata)
        throw RadException("Failed to alloc figure data.");
}

Figure::~Figure() {
    // Required for forward declaration.
}

void Figure::Show(bool block) {
    _pdata->_figure.Show(block);
}

}  // namespace radplot
