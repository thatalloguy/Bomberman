#version 420 core

in vec2 TexCoords;

out vec4 fragColor;

uniform sampler2D tex;

void main() {
    vec4 albedo = texture(tex, TexCoords).rgba;

    fragColor = albedo;
}