// created by Andrey Vasiliev

#include "header.glsl"

in highp vec3 normal;
out highp vec3 vRay;
void main()
{
    vec4 positions[4] = vec4[4](
		vec4(-1.0, -1.0, 0.0, 1.0),
		vec4(1.0, -1.0, 0.0, 1.0),
		vec4(-1.0, 1.0, 0.0, 1.0),
		vec4(1.0, 1.0, 0.0, 1.0));

    gl_Position = positions[gl_VertexID];
    vRay = normal.xyz;
}
