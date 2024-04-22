// created by Andrey Vasiliev

#include "header.glsl"
#include "tonemap.glsl"

uniform sampler2D RT;

in highp vec2 vUV0;
void main()
{
    vec3 color = texture2D(RT, vec2(vUV0)).rgb;
#ifndef FORCE_TONEMAP
    color = unreal(color);
#endif
    FragColor.rgb = SafeHDR(color);
}
