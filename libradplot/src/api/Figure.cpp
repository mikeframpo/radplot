
#include "radplot/Figure.h"
#include "../Radplot.h"

namespace {}  // namespace

namespace radplot {

glm::vec3 ToVec3(Vec3 vec) {
    return {vec.x, vec.y, vec.z};
}

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

void Figure::WaitForClose() {
    _pdata->_figure.WaitForClose();
}

void Figure::DrawQuad(Vec3 centre) {
    auto* renderer = _pdata->_figure.GetRenderer();

    glm::mat4 model(1);
    model = glm::translate(model, ToVec3(centre));

    renderer->DrawQuad(model);
}

}  // namespace radplot
