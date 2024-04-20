// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

// https://github.com/Experience-Monks/glsl-fast-gaussian-blur/blob/5dbb6e97aa43d4be9369bdd88e835f47023c5e2a/9.glsl
vec3 Gauss9(sampler2D tex, const vec2 uv)
{
    vec2 tsize = 1.0 / vec2(textureSize(tex, 0));

    vec3 A = texture2D(tex, uv).rgb;
    vec3 B = texture2D(tex, uv + tsize * vec2(1.3846153846, 0.0)).rgb;
    vec3 C = texture2D(tex, uv - tsize * vec2(1.3846153846, 0.0)).rgb;
    vec3 D = texture2D(tex, uv + tsize * vec2(3.2307692308, 0.0)).rgb;
    vec3 E = texture2D(tex, uv - tsize * vec2(3.2307692308, 0.0)).rgb;

    vec3 c1 = A * 0.2270270270;
    vec3 c2 = (B + C) * 0.3162162162;
    vec3 c3 = (D + E) * 0.0702702703;

    return c1 + c2 + c3;
}

void main()
{
    FragColor.rgb = Gauss9(RT, gl_FragCoord.xy / vec2(textureSize(RT, 0)));
}
