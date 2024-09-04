// created by Andrey Vasiliev

#include "header.glsl"
#include "tonemap.glsl"

uniform sampler2D RT;

void main()
{
    vec2 uv = gl_FragCoord.xy;
    uv.y = textureSize(RT, 0).y - uv.y;
    FragColor.rgb = inverseTonemapSRGB(texelFetch(RT, ivec2(uv), 0).rgb);
}
