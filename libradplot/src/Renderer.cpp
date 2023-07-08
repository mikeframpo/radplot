
#include <GL/glew.h>

#include "Radplot.h"

namespace radplot {

LOG_MODULE(LogModule::Renderer);

struct QuadVertex {
    glm::vec3 Position;
};

template <typename VertexData, int max_models, int vertices_per_model>
struct GeometryData {
    static constexpr int MaxModels = max_models;
    static constexpr int VerticesPerModel = vertices_per_model;

    // cpu buffer
    std::array<VertexData, max_models * vertices_per_model> BufferBase;
    // indices
    IndexBuffer Indices;
    // gpu buffer
    VertexArray VertArray;

    int ModelCount = 0;

    // TODO: template argument for idx count
    void InitBuffers(const std::array<uint32_t, 6>& index_vals, const VertexLayout& layout) {
        // Should this be StaticDraw?
        VertexBuffer vertex_buffer(BufferBase.data(), sizeof(BufferBase), VBDataUsage::StaticDraw);

        std::vector<uint32_t> indices;
        indices.reserve(index_vals.size() * max_models);

        for (int iq = 0; iq < max_models; iq++) {
            for (auto idx : index_vals) {
                indices.push_back(idx);
            }
        }
        Indices = IndexBuffer(indices);

        // bind the layout to the vao
        VertArray = VertexArray::CreateArray(std::move(vertex_buffer), layout);
    }
};

struct Renderer::RenderData {
    // == Quads ==
    static const int MaxQuads = 1;
    static const int VerticesPerQuad = 4;

    GeometryData<QuadVertex, MaxQuads, VerticesPerQuad> QuadData;
};

Renderer::Renderer(Window* window) : _data(std::make_unique<RenderData>()), _camera(window) {
    if (!_data)
        throw RadException("Failed to allocate RenderData");

    LOG_TRACE("sizeof(Renderdata)=%d bytes", sizeof(RenderData));

    glEnable(GL_DEPTH_TEST);

    // Quads (templatized)
    {
        constexpr std::array<uint32_t, 6> quad_indices{{0, 1, 2, 2, 3, 0}};

        VertexLayout layout;
        layout.PushFloatAttr(3, sizeof(QuadVertex), offsetof(QuadVertex, Position));

        _data->QuadData.InitBuffers(quad_indices, layout);
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

    {
        auto& program = Program::LoadProgram("quad");
        program.Bind();

        auto& buffer = _data->QuadData.VertArray.GetVertexBuffer();
        buffer.SetData(_data->QuadData.BufferBase.data(), sizeof(_data->QuadData.BufferBase));

        _data->QuadData.VertArray.Bind();
        _data->QuadData.Indices.Bind();

        // set uniforms
        auto& proj = _camera.GetProjectionMatrix();
        auto& view = _camera.GetViewMatrix();

        program.SetUniformMat4f("proj", proj);
        program.SetUniformMat4f("view", view);

        glDrawElements(GL_TRIANGLES, 6 * _data->QuadData.ModelCount, GL_UNSIGNED_INT, 0);
    }
}

void Renderer::DrawQuad(glm::vec2 size) {
    constexpr glm::vec3 quad_positions[]{
        {-1.0f, -1.0f, 0.0f}, {1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {-1.0f, 1.0f, 0.0f}};

    if (_data->QuadData.ModelCount == _data->QuadData.MaxModels)
        throw RadException("Exceeded max quads");

    QuadVertex* next = &_data->QuadData.BufferBase.at(_data->QuadData.ModelCount * 4);

    // TODO: move vertex copy into Geometry member?

    // Copy positions into the vertices
    for (auto& pos : quad_positions) {
        next->Position = pos;

        next->Position.x *= size.x;
        next->Position.y *= size.y;
        // no scaling in z

        next++;
    }
    _data->QuadData.ModelCount++;
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