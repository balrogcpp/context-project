// created by Andrey Vasiliev

#include "header.glsl"
#include "tonemap.glsl"

uniform sampler2D RT;

float Downscale2x2(sampler2D tex, vec2 uv)
{
    vec2 tsize = 1.0 / vec2(textureSize(tex, 0));
    float A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).x;
    float B = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).x;
    float C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).x;
    float D = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).x;

    float c1 = (A + B + C + D) * 0.25;

    return c1;
}

in vec2 vUV0;
void main()
{
    float lum = Downscale2x2(RT, vUV0);
    FragColor.r = SafeHDR(lum);
}
