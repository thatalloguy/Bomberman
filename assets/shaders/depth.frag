#version 330 core

in vec2 uv;

uniform vec4 filter;
uniform vec4 secondary;
uniform vec4 replace;

uniform sampler2D tex;
uniform vec4 col;

out vec4 fragColor;

void main() {
    vec4 color = texture(tex, uv).rgba;

     if (color.rgb == filter.rgb)
         discard;

    // if (color.rgb == secondary.rgb)
    //     color.rgb = replace.rgb;

    
    fragColor = vec4(0.5, 0, 0, 0.5);
    
}