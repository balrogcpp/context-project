// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

void main()
{
    ivec2 uv = ivec2(gl_FragCoord.xy);
    FragColor = texelFetch(RT,  uv, 0);
}
