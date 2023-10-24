// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

vec3 Downscale2x2(sampler2D tex, const vec2 uv, const vec2 tsize)
{
    vec3 A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).xyz;
    vec3 B = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).xyz;
    vec3 C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).xyz;
    vec3 D = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).xyz;

    vec3 c1 = (A + B + C + D) * 0.25;

    return c1;
}

in vec2 vUV0;
void main()
{
    vec2 TexelSize0 = 1.0 / vec2(textureSize(RT, 0));
    FragColor.rgb = Downscale2x2(RT, vUV0, TexelSize0);
}
