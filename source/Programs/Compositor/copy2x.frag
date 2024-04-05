// created by Andrey Vasiliev

#define HAS_MRT
#include "header.glsl"

uniform sampler2D RT0;
uniform sampler2D RT1;

void main()
{
    FragData[0] = texelFetch(RT0, ivec2(gl_FragCoord.xy), 0);
    FragData[1] = texelFetch(RT1, ivec2(gl_FragCoord.xy), 0);
}
