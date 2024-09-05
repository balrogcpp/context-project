// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

void main()
{
    vec2 uv = gl_FragCoord.xy;
    uv.y = textureSize(RT, 0).y - uv.y;
    FragColor.rgb = texelFetch(RT, ivec2(uv), 0).rgb;
}
