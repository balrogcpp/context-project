// created by Andrey Vasiliev

#include "header.glsl"
#include "mosaic.glsl"

uniform sampler2D RT;
uniform sampler2D SsrTex;
uniform sampler2D GlossTex;

//in vec2 vUV0;
void main()
{
    vec2 texelSize0 = 1.0 / vec2(textureSize(RT, 0));
    vec2 uv0 = gl_FragCoord.xy * texelSize0;
    vec3 color = texture2D(RT, uv0).rgb;
    vec3 ssr = texture2D(SsrTex, uv0).rgb;
    vec2 uv = ssr.xy;
    float fresnel = ssr.z;
    float metallic = texture2D(SsrTex, uv0).r;

    if (Any(uv)) {
        color = mix(color, texture2D(RT, uv).rgb, fresnel * metallic);
    }

    FragColor.rgb = SafeHDR(color);
}
