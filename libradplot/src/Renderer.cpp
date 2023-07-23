
#include <GL/glew.h>

#include "Renderer.h"
#include "Radplot.h"

namespace radplot {

LOG_MODULE(LogModule::Renderer);

struct QuadVertex {
    glm::vec3 Position;
};

struct GridVertex {
    glm::vec3 Position;
};

struct CubeVertex {
    glm::vec3 Position;
};

template <typename VertexData, int max_models, int vertices_per_model, int indices_per_model>
struct GeometryData {
    using VertexType = VertexData;

    static constexpr int MaxModels = max_models;
    static constexpr int VerticesPerModel = vertices_per_model;
    static constexpr int IndicesPerModel = indices_per_model;

    // cpu buffer
    std::array<VertexData, max_models * vertices_per_model> BufferBase;
    // indices
    IndexBuffer Indices;
    // gpu buffer
    VertexArray VertArray;

    int ModelCount = 0;

    // TODO: template argument for idx count
    void InitBuffers(const std::array<uint32_t, indices_per_model>& index_vals, const VertexLayout& layout) {
        // Should this be StaticDraw?
        VertexBuffer vertex_buffer(BufferBase.data(), sizeof(BufferBase), VBDataUsage::StaticDraw);

        std::vector<uint32_t> indices;
        indices.reserve(index_vals.size() * max_models);

        for (int iq = 0; iq < max_models; iq++) {
            int vert_base_idx = iq * vertices_per_model;
            for (auto idx : index_vals) {
                indices.push_back(vert_base_idx + idx);
            }
        }
        Indices = IndexBuffer(indices);

        // bind the layout to the vao
        VertArray = VertexArray::CreateArray(std::move(vertex_buffer), layout);
    }

    const Program& Bind(const char* program_name) {
        auto& program = Program::LoadProgram(program_name);
        program.Bind();

        auto& buffer = VertArray.GetVertexBuffer();
        buffer.SetData(BufferBase.data(), sizeof(BufferBase));

        VertArray.Bind();
        Indices.Bind();

        return program;
    }

    void DrawElements() { glDrawElements(GL_TRIANGLES, IndicesPerModel * ModelCount, GL_UNSIGNED_INT, 0); }
};

struct Renderer::RenderData {
    // == Quads ==
    static const int MaxQuads = 2;
    GeometryData<QuadVertex, MaxQuads, 4, 6> QuadData;

    // == Grids ==
    static const int MaxGrids = 1;
    GeometryData<GridVertex, MaxGrids, 4, 6> GridData;

    // == Cubes
    static const int MaxCubes = 1;
    GeometryData<CubeVertex, MaxCubes, 8, 36> CubeData;
};

Renderer::Renderer(Window* window) : _data(std::make_unique<RenderData>()), _camera(window) {
    if (!_data)
        throw RadException("Failed to allocate RenderData");

    LOG_TRACE("sizeof(Renderdata)=%d bytes", sizeof(RenderData));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Quads
    {
        constexpr std::array<uint32_t, 6> quad_indices{{0, 1, 2, 2, 3, 0}};

        {
            VertexLayout layout;
            layout.PushFloatAttr(3, sizeof(QuadVertex), offsetof(QuadVertex, Position));

            _data->QuadData.InitBuffers(quad_indices, layout);
        }

        {
            VertexLayout layout;
            layout.PushFloatAttr(3, sizeof(GridVertex), offsetof(GridVertex, Position));

            // grid is rendered as a quad covering the whole viewport
            _data->GridData.InitBuffers(quad_indices, layout);
        }
    }

    // Cubes
    {
        constexpr std::array<uint32_t, 36> cube_indices{{0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5, 0, 5, 6, 0, 6, 1,
                                                         1, 6, 7, 1, 7, 2, 7, 4, 3, 7, 3, 2, 4, 7, 6, 4, 6, 5}};

        VertexLayout layout;
        layout.PushFloatAttr(3, sizeof(CubeVertex), offsetof(CubeVertex, Position));

        _data->CubeData.InitBuffers(cube_indices, layout);
    }

    // Default camera position
    auto& view_state = _camera.GetState();
    view_state.Pos = {0.0, 0.0, 5.0};
    view_state.Centre = {0.0, 0.0, 0.0};
    view_state.Up = {0.0, 1.0, 0.0};
}

Renderer::~Renderer() {
    // Destructor required for forward declaration.
    LOG_DEBUG("Renderer destroy");
}

void Renderer::RenderScene() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if 0
    {
        auto& program = _data->CubeData.Bind("cube");

        auto& proj = _camera.GetProjectionMatrix();
        auto& view = _camera.GetViewMatrix();

        program.SetUniformMat4f("proj", proj);
        program.SetUniformMat4f("view", view);

        _data->CubeData.DrawElements();
    }
#endif

#if 1
    {
        auto& program = _data->QuadData.Bind("quad");

        // set uniforms
        auto& proj = _camera.GetProjectionMatrix();
        auto& view = _camera.GetViewMatrix();

        program.SetUniformMat4f("proj", proj);
        program.SetUniformMat4f("view", view);

        _data->QuadData.DrawElements();
    }
#endif

#if 1
    {
        // Render the grid.
        auto& program = _data->GridData.Bind("grid");

        auto& proj = _camera.GetProjectionMatrix();
        auto& view = _camera.GetViewMatrix();

        auto ipv = glm::inverse(proj * view);
        program.SetUniformMat4f("view", view);
        program.SetUniformMat4f("proj", proj);

        _data->GridData.DrawElements();
    }
#endif

}

template <typename TGeometryData>
void DrawQuadTo(TGeometryData& data, const glm::mat4& model) {
    // 0, 1, 2, 2, 3, 0
    constexpr glm::vec3 quad_positions[]{
        {-1.0f, -1.0f, 0.0f}, {1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {-1.0f, 1.0f, 0.0f}};

    if (data.ModelCount == data.MaxModels)
        throw RadException("Exceeded max quads");

    using VertexType = decltype(data.BufferBase)::value_type;
    VertexType* next = &data.BufferBase.at(data.ModelCount * data.VerticesPerModel);

    // TODO: move vertex copy into Geometry member?

    // Copy positions into the vertices
    for (auto& pos : quad_positions) {
        next->Position = model * glm::vec4(pos.x, pos.y, pos.z, 1.0);
        // no scaling in z

        next++;
    }
    data.ModelCount++;
}

void Renderer::DrawQuad(const glm::mat4& model) {
    DrawQuadTo(_data->QuadData, model);
}

void Renderer::DrawGrid() {
    DrawQuadTo(_data->GridData, glm::mat4(1));
}

void Renderer::DrawCube() {
    constexpr glm::vec3 cube_vertices[]{{1.0f, 1.0f, 1.0f},   {-1.0f, 1.0f, 1.0f},  {-1.0f, -1.0f, 1.0f},
                                        {1.0f, -1.0f, 1.0f},  {1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, -1.0f},
                                        {-1.0f, 1.0f, -1.0f}, {-1.0f, -1.0f, -1.0f}};

    if (_data->CubeData.ModelCount == _data->CubeData.MaxModels)
        throw RadException("Exceeded max cubes");

    CubeVertex* next = &_data->CubeData.BufferBase.at(_data->CubeData.ModelCount * _data->CubeData.VerticesPerModel);
    for (auto& pos : cube_vertices) {
        next->Position = pos;

        next++;
    }

    _data->CubeData.ModelCount++;
}
Camera::Camera(Window* window) : _window(window) {}

void Camera::ViewState::Translate(float right, float up) {
    glm::vec3 up_axis = glm::normalize(Up);
    glm::vec3 right_axis = glm::normalize(glm::cross(LookVector(), up_axis));

    glm::vec3 translate = right_axis * right + up_axis * up;
    auto translation = glm::translate(translate);

    Pos = translation * glm::vec4{Pos, 1.0f};
    Centre = translation * glm::vec4{Centre, 1.0f};
}

void Camera::ViewState::Rotate(float yaw, float pitch) {
    // pitch axis is (Up X Look)
    glm::vec3 pitch_axis = glm::cross(Up, LookVector());
    auto rotation = glm::rotate(yaw, glm::normalize(Up)) * glm::rotate(pitch, glm::normalize(pitch_axis));

    // do the rotation
    Pos = rotation * glm::vec4{Pos, 1.0};
}

void Camera::ViewState::Zoom(float dradius) {
    auto lookV = LookVector();
    glm::vec3 look_axis = glm::normalize(lookV);

    float dist = glm::length(lookV);
    float new_dist = glm::max(dist + dradius, 0.0f);

    // don't want to set Pos == Centre.
    if (new_dist > 0)
        Pos = Centre + look_axis * new_dist;
}

const glm::mat4& Camera::GetViewMatrix() {
    _view_matrix = glm::lookAt(_view_state.Pos, _view_state.Centre, _view_state.Up);
    return _view_matrix;
}

const glm::mat4& Camera::GetProjectionMatrix() {
    glm::ivec2 size = _window->GetSize();

    // TODO: cache window size
    float aspect = (float)size.x / (float)size.y;

    _projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
    return _projection;
}

glm::ivec2 Camera::GetWindowSize() const {
    return _window->GetSize();
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