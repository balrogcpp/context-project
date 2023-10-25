// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D Ssao1Tex;
uniform sampler2D Ssao2Tex;

void main()
{
    ivec2 uv = ivec2(gl_FragCoord.xy);
    FragColor.r = SafeHDR(min(texelFetch(Ssao1Tex, uv, 0).r, texelFetch(Ssao2Tex, uv, 0).r));
}
