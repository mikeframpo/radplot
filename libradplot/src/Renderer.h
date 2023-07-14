#pragma once

#include "Window.h"

#include <memory>
#include <glm/glm.hpp>

namespace radplot {

class Camera {
public:
    struct ViewState {
        // Vector from the view Pos to the "lookat" Centre.
        glm::vec3 LookVector() { return Pos - Centre; }

        // Translate by right, up in world. right axis is LookVector * Up
        void Translate(float right, float up);

        // Rotate around the Centre, in the Up axis (yaw), and then rotate the pitch.
        void Rotate(float yaw, float pitch);

        // Move the view's radius by dradius from Centre.
        void Zoom(float dradius);

        glm::vec3 Pos;
        glm::vec3 Centre;
        glm::vec3 Up;
    };

    Camera(Window* window);

    Camera(const Camera& other) = delete;
    Camera& operator=(const Camera& other) = delete;

    ViewState& GetState() { return _view_state; }
    void SetState(const ViewState& view_state) { _view_state = view_state; }

    const glm::mat4& GetViewMatrix();
    const glm::mat4& GetProjectionMatrix();

    glm::vec4 WindowToWorld(int x, int y);

private:
    Window* _window;
    ViewState _view_state;

    glm::mat4 _view_matrix;
    glm::mat4 _projection;
};

class Renderer {
public:
    Renderer(Window* window);
    ~Renderer();

    Camera& GetCamera() { return _camera; }
    void RenderScene();

    void DrawQuad(glm::vec2 size);
    void DrawGrid();
    void DrawCube();

private:
    struct RenderData;
    std::unique_ptr<RenderData> _data;

    Camera _camera;
};

}  // namespace radplot