// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

vec3 Gauss9H(sampler2D tex, vec2 uv, vec2 tsize)
{
    vec3 A = texture2D(tex, uv).xyz;
    vec3 B = texture2D(tex, uv + tsize * vec2(1.3846153846, 0.0)).xyz;
    vec3 C = texture2D(tex, uv - tsize * vec2(1.3846153846, 0.0)).xyz;
    vec3 D = texture2D(tex, uv + tsize * vec2(3.2307692308, 0.0)).xyz;
    vec3 E = texture2D(tex, uv - tsize * vec2(3.2307692308, 0.0)).xyz;

    vec3 c1 = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

    return c1;
}

void main()
{
    vec2 texelSize0 = 1.0 / vec2(textureSize(RT, 0));
    vec2 uv = gl_FragCoord.xy * texelSize0;
    FragColor.rgb = Gauss9H(RT, uv, texelSize0);
}
