// created by Andrey Vasiliev

#include "header.glsl"
#include "tonemap.glsl"

uniform sampler2D RT;

float Downscale2x2(sampler2D tex, const vec2 uv)
{
    vec2 tsize = 1.0 / vec2(textureSize(tex, 0));
    float A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).x;
    float B = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).x;
    float C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).x;
    float D = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).x;

    float c1 = (A + B + C + D) * 0.25;

    return c1;
}

void main()
{
    vec2 uv = gl_FragCoord.xy * 2.0 / vec2(textureSize(RT, 0));
    float lum = Downscale2x2(RT, uv);
    FragColor.r = SafeHDR(lum);
}
