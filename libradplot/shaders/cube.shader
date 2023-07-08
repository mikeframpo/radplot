// vertex
#version 330 core

layout(location = 0) in vec3 position;

out vec3 vert_pos;

uniform mat4 proj;
uniform mat4 view;

void main()
{
    gl_Position = proj * view * vec4(position.xyz, 1.0);
    vert_pos = position;
}

// fragment
#version 330 core

out vec3 color;
in vec3 vert_pos;

void main()
{
    color = vert_pos;
}