// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D RT0;

in highp vec2 vUV0;
void main()
{
    vec3 rt = texture2D(RT, vUV0).rgb;
    vec3 rt0 = texture2D(RT0, vUV0).rgb;
    FragColor.rgb = SafeHDR(rt + rt0 / 7.0);
}
