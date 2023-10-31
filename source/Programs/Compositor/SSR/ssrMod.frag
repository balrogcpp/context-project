// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D SsrTex;
uniform sampler2D GlossTex;

in highp vec2 vUV0;
void main()
{
    vec3 color = texture2D(RT, vUV0).rgb;
    vec3 ssr = texture2D(SsrTex, vUV0).rgb;
    vec2 uv = ssr.xy;
    float fresnel = ssr.z;
    float metallic = texture2D(SsrTex, vUV0).r;

    if (Any(uv)) {
        color = mix(color, texture2D(RT, uv).rgb, fresnel * metallic);
    }

    FragColor.rgb = SafeHDR(color);
}
