// created by Andrey Vasiliev

#include "header.glsl"
#include "SMAA_GLSL.glsl"

#define SMAA_INCLUDE_VS 1
#define SMAA_INCLUDE_PS 0

uniform mat4 worldViewProj;
uniform vec4 ViewportSize;

#include "smaa.glsl"

in highp vec3 vertex;
out vec4 offset;
void main()
{
	gl_Position = worldViewProj * vec4(vertex, 1.0);
	vec2 inPos = sign(vertex.xy);
	vec2 uv = vec2(inPos.x, -inPos.y) * 0.5 + 0.5;
	SMAANeighborhoodBlendingVS(uv, offset);
}
