#pragma once

#include <memory>
#include <glm/glm.hpp>

namespace radplot {

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    void RenderScene();

    void DrawQuad(glm::vec2 size);

private:
    struct RenderData;
    std::unique_ptr<RenderData> _data;
};

}  // namespace radplot