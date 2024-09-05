// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

void main()
{
    ivec2 uv = ivec2(gl_FragCoord.xy * 2.0);
    uv.y = textureSize(RT, 0).y * 2 - uv.y - 1;
    FragColor = texelFetch(RT, uv, 0);
}
