// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

// https://github.com/Experience-Monks/glsl-fast-gaussian-blur/blob/5dbb6e97aa43d4be9369bdd88e835f47023c5e2a/9.glsl
vec3 Gauss9Y(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
    vec2 off1 = tsize * vec2(0.0, 1.3846153846);
    vec2 off2 = tsize * vec2(0.0, 3.2307692308);
    vec3 A = textureLod(tex, uv, 0.0).rgb;
    vec3 B = textureLod(tex, uv + off1, 0.0).rgb;
    vec3 C = textureLod(tex, uv - off1, 0.0).rgb;
    vec3 D = textureLod(tex, uv + off2, 0.0).rgb;
    vec3 E = textureLod(tex, uv - off2, 0.0).rgb;

    vec3 c = A * 0.2270270270;
    c += (B + C) * 0.3162162162;
    c += (D + E) * 0.0702702703;

    return c;
}

void main()
{
    vec2 size = vec2(textureSize(RT, 0));
    vec2 tsize = 1.0 / size;
    vec2 uv = gl_FragCoord.xy / size;
    uv.y = 1.0 - uv.y;

    FragColor.rgb = Gauss9Y(RT, uv, tsize);
}
