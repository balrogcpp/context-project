// created by Andrey Vasiliev

#include "header.glsl"
#include "tonemap.glsl"

uniform sampler2D RT;
//uniform float Exposure;

in highp vec2 vUV0;
void main()
{
    vec3 color = texelFetch(RT, ivec2(vUV0 * vec2(textureSize(RT, 0))), 0).rgb;
#ifndef FORCE_TONEMAP
    color = unreal(color);
#endif
    FragColor.rgb = SafeHDR(color);
}
