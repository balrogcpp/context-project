// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D SsaoTex;
uniform vec4 ShadowColour;

float Gauss9V(sampler2D tex, vec2 uv, vec2 tsize)
{
    float A = texture2D(tex, uv).x;
    float B = texture2D(tex, uv + tsize * vec2(0.0, 1.3846153846)).x;
    float C = texture2D(tex, uv - tsize * vec2(0.0, 1.3846153846)).x;
    float D = texture2D(tex, uv + tsize * vec2(0.0, 3.2307692308)).x;
    float E = texture2D(tex, uv - tsize * vec2(0.0, 3.2307692308)).x;

    float c1 = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

    return c1;
}

void main()
{
    vec2 texelSize0 = 1.0 / vec2(textureSize(RT, 0));
    vec2 uv = gl_FragCoord.xy * texelSize0;
    vec3 color = texture2D(RT, uv).rgb;
    float ssao = Gauss9V(SsaoTex, uv, texelSize0);

    color *= clamp(ssao + ShadowColour.g, 0.0, 1.0);
    FragColor.rgb = SafeHDR(color);
}
