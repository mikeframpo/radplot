
#include <GL/glew.h>
#include "Radplot.h"

#include "Renderer.h"

namespace radplot {

LOG_MODULE(LogModule::Renderer);

struct QuadVertex {
    glm::vec3 Position;
};

template<typename V>
struct GeometryData {

};

struct Renderer::RenderData {

    // == Quads ==
    static const int MaxQuads = 1;
    static const int VerticesPerQuad = 4;

    // CPU buffer in fixed size array
    std::array<QuadVertex, MaxQuads * VerticesPerQuad> QuadsBufferBase;
    IndexBuffer QuadsIndices;
    VertexArray QuadsVertexArray;
    Shader QuadShader;
    int QuadsCount = 0;
};

Renderer::Renderer(Window* window) : _data(std::make_unique<RenderData>()), _camera(window) {
    if (!_data)
        throw RadException("Failed to allocate RenderData");

    LOG_TRACE("sizeof(Renderdata)=%d bytes", sizeof(RenderData));

    glEnable(GL_DEPTH_TEST);

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

    // Default camera position
    _camera.SetPosition({0.0, 0.0, 4.0});
    _camera.SetDirection({0.0, 0.0, -1.0});
    _camera.SetUp({0.0, 1.0, 0.0});
}

Renderer::~Renderer() {
    // Destructor required for forward declaration.
    LOG_DEBUG("Renderer destroy");
}

void Renderer::RenderScene() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Quads
    {
        auto& program = Program::LoadProgram("quad");
        program.Bind();

        auto& buffer = _data->QuadsVertexArray.GetVertexBuffer();
        buffer.SetData(_data->QuadsBufferBase.data(), sizeof(_data->QuadsBufferBase));

        _data->QuadsVertexArray.Bind();
        _data->QuadsIndices.Bind();

        // set uniforms
        auto& proj = _camera.GetProjectionMatrix();
        auto& view = _camera.GetViewMatrix();

        program.SetUniformMat4f("proj", proj);
        program.SetUniformMat4f("view", view);

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
        // no scaling in z

        next++;
    }
    _data->QuadsCount++;
}

Camera::Camera(Window* window) : _window(window) {}

const glm::mat4& Camera::GetViewMatrix() {
    _view = glm::lookAt(_pos, _pos + _direction, _up);
    return _view;
}

const glm::mat4& Camera::GetProjectionMatrix() {
    glm::ivec2 size = _window->GetSize();

    // TODO: cache window size
    float aspect = (float)size.x / (float)size.y;

    _projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f);
    return _projection;
}

glm::vec4 Camera::WindowToWorld(int x, int y) {
    glm::ivec2 size = _window->GetSize();

    // convert to clip space
    float xclip = 2.0f * (float)x / (float)size.x - 1.0f;
    float yclip = 1.0f - 2.0f * (float)y / (float)size.y;

    auto& view = GetViewMatrix();
    auto& proj = GetProjectionMatrix();

    // inverse project view/model matrices
    glm::mat4 ivp = glm::inverse(proj * view);

    // find the pixel depth
    float zdepth;
    // TODO: depth doesn't seem to read reliably after panning, what's going on?
    glReadPixels(x, size.y - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zdepth);
    // https://stackoverflow.com/questions/7692988
    float zclip = 2.0f * zdepth - 1.0;
    glm::vec4 clip(xclip, yclip, zclip, 1.0);

    glm::vec4 world = ivp * clip;
    world.w = 1.0 / world.w;

    world.x *= world.w;
    world.y *= world.w;
    world.z *= world.w;

    return world;
}

}  // namespace radplot