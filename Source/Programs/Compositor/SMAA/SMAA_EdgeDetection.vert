// created by Andrey Vasiliev

#include "header.glsl"
#include "SMAA_GLSL.glsl"

uniform vec4 ViewportSize;

#define SMAA_INCLUDE_VS 1
#define SMAA_INCLUDE_PS 0
#include "smaa.glsl"

out vec4 offset[3];
void main()
{
    vec4 positions[4] = vec4[4](
		vec4(-1.0, -1.0, 0.0, 1.0),
		vec4(1.0, -1.0, 0.0, 1.0),
		vec4(-1.0, 1.0, 0.0, 1.0),
		vec4(1.0, 1.0, 0.0, 1.0));

    gl_Position = positions[gl_VertexID];

	SMAAEdgeDetectionVS(gl_Position.xy, offset);
}
