// created by Andrey Vasiliev

#include "header.glsl"
#include "SMAA_GLSL.glsl"

#define SMAA_INCLUDE_VS 1
#define SMAA_INCLUDE_PS 0

uniform mat4 worldViewProj;
uniform vec4 ViewportSize;

#include "smaa.glsl"

in highp vec3 vertex;
out vec2 vUV0;
out vec2 pixcoord0;
out vec4 offset[3];
void main()
{
	gl_Position = worldViewProj * vec4(vertex, 1.0);
	vec2 inPos = sign(vertex.xy);
	vUV0 = vec2(inPos.x, -inPos.y) * 0.5 + 0.5;
	SMAABlendingWeightCalculationVS(vUV0, pixcoord0, offset);
}
