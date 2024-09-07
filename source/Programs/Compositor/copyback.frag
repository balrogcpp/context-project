// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

void main()
{
    vec2 uv = gl_FragCoord.xy / vec2(textureSize(RT, 0));
    uv.y = 1.0 - uv.y;

    FragColor = textureLod(RT, uv, 0.0);
}
