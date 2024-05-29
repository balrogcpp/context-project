// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D BrightTex;
uniform sampler2D Lum;

in highp vec2 vUV0;
void main()
{
    float lum = texelFetch(Lum, ivec2(0, 0), 0).r;
    vec3 bright = textureLod(BrightTex, vUV0, 0.0).rgb;
    vec3 color = textureLod(RT, vUV0, 0.0).rgb * lum + bright;
    FragColor.rgb = unreal(color);
}
