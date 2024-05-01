// created by Andrey Vasiliev

#include "header.glsl"
#ifndef FORCE_TONEMAP
#include "tonemap.glsl"
#endif

uniform sampler2D RT;

in highp vec2 vUV0;
void main()
{
#ifndef FORCE_TONEMAP
    FragColor.rgb = SafeHDR(unreal(texture2D(RT, vUV0).rgb));
#else
    FragColor.rgb = SafeHDR(texture2D(RT, vUV0).rgb);
#endif
}
