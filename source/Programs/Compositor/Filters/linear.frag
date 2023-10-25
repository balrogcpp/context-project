// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

void main()
{
    FragColor.rgb = texelFetch(RT, ivec2(gl_FragCoord.xy) << 1, 0).rgb;
}
