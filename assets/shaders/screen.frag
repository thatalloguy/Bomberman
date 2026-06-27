
//CREDITS TO: https://github.com/wessles/GLSL-CRT/blob/master/shader.frag
#version 330 core

#define CRT_CURVE_AMNTx 0.0 // curve amount on x
#define CRT_CURVE_AMNTy 0.0 // curve amount on y
#define CRT_CASE_BORDR 1.0125
#define SCAN_LINE_MULT 00.0

in vec2 uv;
out vec4 fragColor;

uniform sampler2D screen;


void main() {
	vec2 tc = vec2(uv.x, uv.y);

	// Distance from the center
	float dx = abs(0.5-tc.x);
	float dy = abs(0.5-tc.y);

	// Square it to smooth the edges
	dx *= dx;
	dy *= dy;

	tc.x -= 0.5;
	tc.x *= 1.0 + (dy * CRT_CURVE_AMNTx);
	tc.x += 0.5;

	tc.y -= 0.5;
	tc.y *= 1.0 + (dx * CRT_CURVE_AMNTy);
	tc.y += 0.5;

	// Get texel, and add in scanline if need be
	vec4 cta = texture2D(screen, vec2(tc.x, tc.y));

	cta.rgb += sin(tc.y * SCAN_LINE_MULT) * 0.2;

	// Cutoff
	if(tc.y > 1.0 || tc.x < 0.0 || tc.x > 1.0 || tc.y < 0.0)
		cta = vec4(0.0);

	if (uv.x > 0.99) cta = vec4(0.5, 0.5, 0.5, 1.0);
	// Apply
	fragColor = cta;
}