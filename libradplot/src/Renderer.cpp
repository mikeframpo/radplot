#include "Renderer.h"

#include <GL/glew.h>
#include "Radplot.h"

namespace radplot {

LOG_MODULE(LogModule::Renderer);

struct QuadVertex {
    glm::vec3 Position;
};

struct Renderer::RenderData {
    static const int MaxQuads = 1;
    static const int VerticesPerQuad = 4;

    // CPU buffer in fixed size array
    std::array<QuadVertex, MaxQuads * VerticesPerQuad> QuadsBufferBase;
    IndexBuffer QuadsIndices;
    VertexArray QuadsVertexArray;
    Shader QuadShader;
    int QuadsCount = 0;
};

Renderer::Renderer() : _data(std::make_unique<RenderData>()) {
    if (!_data)
        throw RadException("Failed to allocate RenderData");

    LOG_TRACE("sizeof(Renderdata)=%d bytes", sizeof(RenderData));

    // Quads
    {
        // Should this be StaticDraw?
        VertexBuffer vertex_buffer(_data->QuadsBufferBase.data(), sizeof(_data->QuadsBufferBase),
                                   VBDataUsage::StaticDraw);

        constexpr std::array<uint32_t, 6> quad_indices{{0, 1, 2, 2, 3, 0}};

        std::vector<uint32_t> indices;
        indices.reserve(quad_indices.size() * RenderData::MaxQuads);

        for (int iq = 0; iq < RenderData::MaxQuads; iq++) {
            for (auto idx : quad_indices) {
                indices.push_back(idx);
            }
        }
        _data->QuadsIndices = IndexBuffer(indices);

        VertexLayout layout;
        layout.PushFloatAttr(3, sizeof(QuadVertex), offsetof(QuadVertex, Position));

        _data->QuadsVertexArray = VertexArray::CreateArray(std::move(vertex_buffer), layout);
    }
}

Renderer::~Renderer() {
    // Required for forward declaration.
}

void Renderer::RenderScene() {

    // Quads
    {
        auto& program = Program::LoadProgram("quad");
        program.Bind();

        auto& buffer = _data->QuadsVertexArray.GetVertexBuffer();
        buffer.SetData(_data->QuadsBufferBase.data(), sizeof(_data->QuadsBufferBase));

        _data->QuadsVertexArray.Bind();
        _data->QuadsIndices.Bind();

        glDrawElements(GL_TRIANGLES, 6 * _data->QuadsCount, GL_UNSIGNED_INT, 0);
    }
}

void Renderer::DrawQuad(glm::vec2 size) {
    constexpr glm::vec3 quad_positions[]{
        {-1.0f, -1.0f, 0.0f}, {1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {-1.0f, 1.0f, 0.0f}};

    if (_data->QuadsCount == RenderData::MaxQuads)
        throw RadException("Exceeded max quads");

    QuadVertex* next = &_data->QuadsBufferBase.at(_data->QuadsCount * 4);

    // Copy positions into the vertices
    for (auto& pos : quad_positions) {
        next->Position = pos;

        next->Position.x *= size.x;
        next->Position.y *= size.y;

        next++;
    }
    _data->QuadsCount++;
}

}  // namespace radplot