// created by Andrey Vasiliev

#define HAS_MRT
#include "header.glsl"

uniform sampler2D RT0;
uniform sampler2D RT1;

in highp vec2 vUV0;
void main()
{
    FragData[0] = texture2D(RT0, vUV0);
    FragData[1] = texture2D(RT1, vUV0);
}
