// created by Andrey Vasiliev

#include "header.glsl"
#include "tonemap.glsl"

uniform sampler2D RT;
uniform sampler2D Lum;

void main()
{
    vec3 color = texelFetch(RT, ivec2(gl_FragCoord.xy), 0).rgb;
    float lum = texelFetch(Lum, ivec2(0, 0), 0).r;
    color *= lum;
    FragColor.rgb = SafeHDR(color);
}
