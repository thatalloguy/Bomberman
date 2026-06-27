#version 330 core

in vec4 p;

out vec2 uv;
out vec3 pos;

uniform vec2 grid;
uniform vec2 frame;

uniform mat4 matrix;
uniform mat4 proj;

uniform float layer;

void main() {
    uv = vec2((p.x + 1) / 2, (p.y + 1) / 2);

    vec2 tileUv = (uv / (grid.y / grid.x));
    tileUv = tileUv += (frame / (grid.y / grid.x));

    uv = tileUv;

    pos = vec3(p.x, p.y, 1);
    gl_Position = proj * matrix * vec4(p.x, p.y, 0, 1);
}
