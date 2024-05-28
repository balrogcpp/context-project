// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D Lum;

in highp vec2 vUV0;
void main()
{
    vec3 color = texture2D(RT, vUV0).rgb;
    float lum = texelFetch(Lum, ivec2(0, 0), 0).r;
    FragColor.rgb = SafeHDR(color * lum);
}
