#version 330 core

in vec4 p;

out vec2 uv;
out vec3 pos;

uniform vec2 frame;

uniform mat4 matrix;
uniform mat4 proj;

void main() {

    uv = vec2((p.x + 1) / 2, 1 - (p.y + 1) / 2);

    pos = vec3(p.x, p.y, 1);
    gl_Position = proj * matrix * vec4(p.x, p.y, 1, 1);
}
