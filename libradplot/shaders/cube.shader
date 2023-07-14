// vertex
#version 330 core

layout(location = 0) in vec3 position;

out vec3 vertPos;

uniform mat4 proj;
uniform mat4 view;

void main()
{
    gl_Position = proj * view * vec4(position.xyz, 1.0);
    vertPos = position;
}

// fragment
#version 330 core

in vec3 vertPos;

out vec4 color;

void main()
{
    float edgeW = 0.05;
    bool xedge = abs(vertPos.x) + edgeW > 1;
    bool yedge = abs(vertPos.y) + edgeW > 1;
    bool zedge = abs(vertPos.z) + edgeW > 1;

    float alpha = 1.0;
    if ((xedge && yedge) || (yedge && zedge) || (xedge && zedge)) {
        color = vec4(0, 0, 0, alpha);
    } else {
        color = vec4(0.3, 0.3, 0.3, alpha);
    }
}