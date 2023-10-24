// created by Andrey Vasiliev

#include "header.glsl"
#include "SMAA_GLSL.glsl"

#define SMAA_INCLUDE_VS 1
#define SMAA_INCLUDE_PS 0

uniform mat4 worldViewProj;
uniform vec4 ViewportSize;

#include "smaa.glsl"

in highp vec4 vertex;
in highp vec4 uv0;
out vec2 vUV0;
out mat4 offset;
void main()
{
	gl_Position = worldViewProj * vertex;
	vUV0 = uv0.xy;
	SMAAEdgeDetectionVS( uv0.xy, offset );
}
