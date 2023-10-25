// created by Andrey Vasiliev

#include "header.glsl"
#include "mosaic.glsl"

uniform sampler2D SsrTex1;
uniform sampler2D SsrTex2;

void main()
{
    ivec2 uv = ivec2(gl_FragCoord.xy * vec2(0.5, 1.0));
    vec3 ssr = max(texelFetch(SsrTex1, uv, 0).rgb, texelFetch(SsrTex2, uv, 0).rgb);
    if (Any(ssr)) FragColor.rgb = ssr;
}
