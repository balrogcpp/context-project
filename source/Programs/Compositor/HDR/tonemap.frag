// created by Andrey Vasiliev

#include "header.glsl"
#include "tonemap.glsl"

uniform sampler2D RT;
//uniform float Exposure;

in highp vec2 vUV0;
void main()
{
    vec3 color = texture2D(RT, vUV0).rgb;
#ifndef FORCE_TONEMAP
#ifndef GL_ES
    color = unreal(expose(color, 2.5));
#else
    color = unreal(expose(color, 4.0));
#endif
#endif
    FragColor.rgb = SafeHDR(color);
}
