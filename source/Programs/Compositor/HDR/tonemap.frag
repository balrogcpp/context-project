// created by Andrey Vasiliev

#include "header.glsl"
#include "tonemap.glsl"

uniform sampler2D RT;

void main()
{
    ivec2 tsize = textureSize(RT, 0);
    ivec2 uv = ivec2(gl_FragCoord.xy);
    uv.y = tsize.y - uv.y;

    vec3 color = texelFetch(RT, uv, 0).rgb;
    color *= 2.0;
    FragColor.rgb = SafeHDR(unreal(color));
}
