// vertex
#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 proj;
uniform mat4 view;

void main()
{
    gl_Position = proj * view * vec4(position, 1.0);
}

// fragment
#version 330 core

out vec4 color;

void main()
{
    color = vec4(0, 1, 0, 0.5);
}