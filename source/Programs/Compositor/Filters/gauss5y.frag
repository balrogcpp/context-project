// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

// https://github.com/Experience-Monks/glsl-fast-gaussian-blur/blob/5dbb6e97aa43d4be9369bdd88e835f47023c5e2a/5.glsl
vec3 Gauss5(sampler2D tex, const vec2 uv)
{
    vec2 tsize = 1.0 / vec2(textureSize(tex, 0));

    vec3 A = texture2D(tex, uv).rgb;
    vec3 B = texture2D(tex, uv + tsize * vec2(0.0, 1.3333333333333333)).rgb;
    vec3 C = texture2D(tex, uv - tsize * vec2(0.0, 1.3333333333333333)).rgb;

    vec3 c1 = A * 0.29411764705882354;
    vec3 c2 = (B + C) * 0.35294117647058826;

    return c1 + c2;
}

void main()
{
    FragColor.rgb = Gauss5(RT, gl_FragCoord.xy / vec2(textureSize(RT, 0)));
}
