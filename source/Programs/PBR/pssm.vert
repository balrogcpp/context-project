// created by Andrey Vasiliev

#include "header.glsl"

uniform highp mat4 WorldViewProjMatrix;

in highp vec4 vertex;
#ifdef SHADOWCASTER_ALPHA
in vec4 uv0;
out highp vec2 vUV0;
#endif
void main()
{
#ifdef SHADOWCASTER_ALPHA
    vUV0.xy = uv0.xy;
#endif

    gl_Position = mul(WorldViewProjMatrix, vertex);
}
