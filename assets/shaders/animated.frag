#version 330 core

in vec2 uv;

uniform vec4 filter;
uniform vec4 secondary;
uniform vec4 replace;

uniform sampler2D tex;

out vec4 fragColor;

void main() {
    vec4 color = texture(tex, uv).rgba;

    if (color.rgb == filter.rgb)
        discard;

    if (color.rgb == secondary.rgb)
        color.rgb = replace.rgb;

    
    fragColor = vec4(color.r, color.g, color.b, 1);
}