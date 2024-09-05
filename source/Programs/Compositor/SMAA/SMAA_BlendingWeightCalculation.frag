// created by Andrey Vasiliev

#include "header.glsl"
#include "SMAA_GLSL.glsl"

#define SMAA_INCLUDE_VS 0
#define SMAA_INCLUDE_PS 1

uniform sampler2D edgeTex;
uniform sampler2D areaTex;
uniform sampler2D searchTex;
uniform vec4 ViewportSize;

#include "smaa.glsl"

in vec2 pixcoord0;
in vec4 offset[3];
void main()
{
	vec2 size = vec2(textureSize(edgeTex, 0));
    vec2 tsize = 1.0 / size;
    vec2 uv = gl_FragCoord.xy / size;
    uv.y = 1.0 - uv.y;

	FragColor = SMAABlendingWeightCalculationPS(uv, pixcoord0, offset, edgeTex, areaTex, searchTex, vec4(0.0, 0.0, 0.0, 0.0));
}
