// created by Andrey Vasiliev
//? #version 400

uniform sampler2D RT;

// https://github.com/Experience-Monks/glsl-fast-gaussian-blur/blob/5dbb6e97aa43d4be9369bdd88e835f47023c5e2a/5.glsl
vec3 Gauss5(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
    vec2 off = tsize * vec2(0.0, 1.3333333333333333);
    vec3 A = textureLod(tex, uv, 0.0).rgb;
    vec3 B = textureLod(tex, uv + off, 0.0).rgb;
    vec3 C = textureLod(tex, uv - off, 0.0).rgb;

    vec3 o = A * 0.29411764705882354;
    o += (B + C) * 0.35294117647058826;

    return o;
}

out vec3 FragColor;
void main()
{
    vec2 tsize = 1.0 / vec2(textureSize(RT, 0));
    vec2 uv = gl_FragCoord.xy * tsize;

    FragColor.rgb = Gauss5(RT, uv, tsize);
}
