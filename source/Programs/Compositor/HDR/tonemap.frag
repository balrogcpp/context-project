// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

in highp vec2 vUV0;
void main()
{
#ifndef FORCE_TONEMAP
    FragColor.rgb = SafeHDR(unreal(textureLod(RT, vUV0, 0.0).rgb));
#else
    FragColor.rgb = SafeHDR(textureLod(RT, vUV0, 0.0).rgb);
#endif
}
