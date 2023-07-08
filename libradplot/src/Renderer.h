#pragma once

#include "Window.h"

#include <memory>
#include <glm/glm.hpp>

namespace radplot {

class Camera {
public:
    Camera(Window* window);

    Camera(const Camera& other) = delete;
    Camera& operator=(const Camera& other) = delete;

    glm::vec3 GetPosition() { return _pos; }
    void SetPosition(glm::vec3 pos) { _pos = pos; }

    void SetDirection(glm::vec3 direction) { _direction = direction; }
    void SetUp(glm::vec3 up) { _up = up; }

    const glm::mat4& GetViewMatrix();
    const glm::mat4& GetProjectionMatrix();

    glm::vec4 WindowToWorld(int x, int y);

private:
    Window* _window;
    glm::vec3 _pos;
    glm::vec3 _up;
    glm::vec3 _direction;

    glm::mat4 _view;
    glm::mat4 _projection;
};

class Renderer {
public:
    Renderer(Window* window);
    ~Renderer();

    Camera& GetCamera() { return _camera; }
    void RenderScene();

    void DrawQuad(glm::vec2 size);
    void DrawCube();

private:
    struct RenderData;
    std::unique_ptr<RenderData> _data;

    Camera _camera;
};

}  // namespace radplot