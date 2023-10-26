// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D Ssao1Tex;
uniform sampler2D Ssao2Tex;

in vec2 vUV0;
void main()
{
    FragColor.r = SafeHDR(min(texture2D(Ssao1Tex, vUV0).r, texture2D(Ssao2Tex, vUV0).r));
}
