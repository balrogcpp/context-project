// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D Lum;
uniform vec2 TexelSize;
uniform vec3 Exposure;
uniform float timeSinceLast;

float Downscale2x2(sampler2D tex, const vec2 uv, const vec2 tsize)
{
    float A = textureLod(tex, uv + tsize * vec2(-1.0, -1.0), 0.0).x;
    float B = textureLod(tex, uv + tsize * vec2(-1.0,  1.0), 0.0).x;
    float C = textureLod(tex, uv + tsize * vec2( 1.0, -1.0), 0.0).x;
    float D = textureLod(tex, uv + tsize * vec2( 1.0,  1.0), 0.0).x;

    float c1 = (A + B + C + D) * 0.25;

    return c1;
}

in highp vec2 vUV0;
void main()
{
    float newLum = Downscale2x2(RT, vUV0, TexelSize);
    newLum = expose(newLum, Exposure);
    float oldLum = textureLod(Lum, vec2(0.0, 0.0), 0.0).r;
    float lum = mix(newLum, oldLum, pow(0.25, timeSinceLast));
    FragColor.r = SafeHDR(lum);
}
