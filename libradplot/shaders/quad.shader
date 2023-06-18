// vertex
#version 330 core

layout(location = 0) in vec3 position;

void main()
{
    gl_Position = vec4(position.xyz, 1.0);
}

// fragment
#version 330 core

out vec3 color;

void main()
{
    color = vec3(1, 0, 0);
}