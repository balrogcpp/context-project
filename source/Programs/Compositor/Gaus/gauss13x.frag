// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

// https://github.com/Experience-Monks/glsl-fast-gaussian-blur/blob/5dbb6e97aa43d4be9369bdd88e835f47023c5e2a/13.glsl
vec3 Gauss13(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
    vec2 off1 = tsize * vec2(1.411764705882353, 0.0);
    vec2 off2 = tsize * vec2(3.2941176470588234, 0.0);
    vec2 off3 = tsize * vec2(5.176470588235294, 0.0);
    vec3 A = textureLod(tex, uv, 0.0).rgb;
    vec3 B = textureLod(tex, uv + off1, 0.0).rgb;
    vec3 C = textureLod(tex, uv - off1, 0.0).rgb;
    vec3 D = textureLod(tex, uv + off2, 0.0).rgb;
    vec3 E = textureLod(tex, uv - off2, 0.0).rgb;
    vec3 F = textureLod(tex, uv + off3, 0.0).rgb;
    vec3 G = textureLod(tex, uv - off3, 0.0).rgb;

    vec3 c = A * 0.1964825501511404;
    c += (B + C) * 0.2969069646728344;
    c += (D + E) * 0.09447039785044732;
    c += (F + G) * 0.010381362401148057;

    return c;
}

void main()
{
    vec2 size = vec2(textureSize(RT, 0));
    vec2 tsize = 1.0 / size;
    vec2 uv = gl_FragCoord.xy / size;
    uv.y = 1.0 - uv.y;

    FragColor.rgb = Gauss13(RT, uv, tsize);
}
