// created by Andrey Vasiliev

#include "header.glsl"
#include "mosaic.glsl"

uniform sampler2D SsaoTex;
uniform sampler2D SsaoTex2;

in vec2 vUV0;
void main()
{
    FragColor.r = SafeHDR(min(texture2D(SsaoTex, vUV0).r, texture2D(SsaoTex2, vUV0).r));
}
