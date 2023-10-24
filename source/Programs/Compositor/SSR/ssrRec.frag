// created by Andrey Vasiliev

#include "header.glsl"
#include "mosaic.glsl"

uniform sampler2D SsrTex1;
uniform sampler2D SsrTex2;

in vec2 vUV0;
void main()
{
    vec3 ssr = max(texture2D(SsrTex1, vUV0).rgb, texture2D(SsrTex2, vUV0).rgb);
    if (Any(ssr)) FragColor.rgb = ssr;
}
