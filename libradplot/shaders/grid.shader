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

vec3 getViewPos(vec3 worldPos) {
    vec4 viewPos = view * vec4(worldPos, 1.0);
    return viewPos.xyz / viewPos.w;
}

vec4 grid(vec3 worldPos, float scale) {
    vec4 colour = vec4(0.2, 0.2, 0.2, 0);

    // coord.y is actually z-axis :-O
    vec2 coord = worldPos.xz;

    // world-space derivative in xz plane, dw/dpx
    vec2 derivative = fwidth(coord);
    float linepx = 1.0;

    // grid line is 1.0 wide centred on 0 and multiples of scale.
    // But in pixel coords
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float minDist = min(grid.x, grid.y);

    colour.a = 1 - smoothstep(0, linepx, minDist);

    float fadeDepth = 50;
    vec3 viewPos = getViewPos(worldPos);

    float fading = 1 - smoothstep(0, fadeDepth, length(viewPos));
    colour.a *= fading;

    return colour;
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