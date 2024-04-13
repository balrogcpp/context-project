// created by Andrey Vasiliev

#include "header.glsl"
#include "tonemap.glsl"

uniform sampler2D RT;
//uniform float Exposure;

void main()
{
    vec3 color = texelFetch(RT, ivec2(gl_FragCoord.xy), 0).rgb;
#ifndef FORCE_TONEMAP
    color = unreal(color);
#endif
    FragColor.rgb = SafeHDR(color);
}
