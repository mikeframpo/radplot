// vertex
#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 view;
uniform mat4 proj;

out vec3 nearPoint;
out vec3 farPoint;

vec3 UnprojectPoint(vec2 planePos, float z, mat4 ivp) {
    vec4 unproject = ivp * vec4(planePos, z, 1.0);
    return unproject.xyz / unproject.w;
}

void main()
{
    mat4 ipv = inverse(proj * view);
    nearPoint = UnprojectPoint(position.xy, -1.0, ipv);
    farPoint = UnprojectPoint(position.xy, 1.0, ipv);

    gl_Position = vec4(position, 1.0);
}

// fragment
#version 330 core

in vec3 nearPoint;
in vec3 farPoint;

uniform mat4 view;
uniform mat4 proj;

out vec4 fragColor;

vec4 grid(vec3 worldPos, float scale) {
    float width = 0.05;

    vec4 color;
    if (abs(worldPos.x) < width) {
        color = vec4(0, 0, 1, 0);
    } else if (abs(worldPos.z) < width) {
        color = vec4(1, 0, 0, 0);
    } else {
        color = vec4(0, 0, 0, 0);
    }


    vec2 grid = fract(abs(worldPos.xz));
    // grid.y is actually z-plane :-O
    if (grid.x < width || grid.y < width) {
        color.a = 1.0;
    }

    return color;
}

float getFragDepth(vec3 worldPos) {
    vec4 clipPos = proj * view * vec4(worldPos, 1.0);
    return (clipPos.z / clipPos.w + 1) / 2;
}

void main()
{
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 worldPos = nearPoint + (farPoint - nearPoint) * t;
    gl_FragDepth = getFragDepth(worldPos);

    if (t > 0) {
        fragColor = grid(worldPos, 1);
    } else {
        fragColor = vec4(0);
    }
}