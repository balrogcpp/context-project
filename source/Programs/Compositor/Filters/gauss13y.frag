// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

// https://github.com/Experience-Monks/glsl-fast-gaussian-blur/blob/5dbb6e97aa43d4be9369bdd88e835f47023c5e2a/13.glsl
vec3 Gauss13(sampler2D tex, const vec2 uv)
{
    vec2 tsize = 1.0 / vec2(textureSize(tex, 0));
    vec3 A = texture2D(tex, uv).xyz;
    vec3 B = texture2D(tex, uv + tsize * vec2(0.0, 1.411764705882353)).xyz;
    vec3 C = texture2D(tex, uv - tsize * vec2(0.0, 1.411764705882353)).xyz;
    vec3 D = texture2D(tex, uv + tsize * vec2(0.0, 3.2941176470588234)).xyz;
    vec3 E = texture2D(tex, uv - tsize * vec2(0.0, 3.2941176470588234)).xyz;
    vec3 F = texture2D(tex, uv - tsize * vec2(0.0, 5.176470588235294)).xyz;
    vec3 G = texture2D(tex, uv - tsize * vec2(0.0, 5.176470588235294)).xyz;

    vec3 c1 = A * 0.1964825501511404 + (B + C) * 0.2969069646728344 + (D + E) * 0.09447039785044732 + (F + G) * 0.010381362401148057;

    return c1;
}

in highp vec2 vUV0;
void main()
{
    FragColor.rgb = Gauss13(RT, vUV0);
}
