// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform vec2 TexelSize;

float Downscale2x2(sampler2D tex, const vec2 uv, const vec2 tsize)
{
    float A = texture2DLod(tex, uv + tsize * vec2(-1.0, -1.0), 0).x;
    float B = texture2DLod(tex, uv + tsize * vec2(-1.0,  1.0), 0).x;
    float C = texture2DLod(tex, uv + tsize * vec2( 1.0, -1.0), 0).x;
    float D = texture2DLod(tex, uv + tsize * vec2( 1.0,  1.0), 0).x;

    float c1 = (A + B + C + D) * 0.25;

    return c1;
}

in highp vec2 vUV0;
void main()
{
    float lum = Downscale2x2(RT, vUV0, TexelSize);
    FragColor.r = SafeHDR(lum);
}
