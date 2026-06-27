#version 330 core

in vec2 uv;

uniform sampler2D tex;

out vec4 fragColor;

void main() {
    vec4 color = texture(tex, uv).rgba;
    fragColor = vec4(texture(tex, uv).rgb, 1);
}