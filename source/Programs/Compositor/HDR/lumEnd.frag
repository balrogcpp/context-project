// created by Andrey Vasiliev

#include "header.glsl"
#include "tonemap.glsl"

uniform sampler2D RT;
uniform sampler2D Lum;
uniform vec3 Exposure;
uniform float timeSinceLast;

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

float expose(float color, const vec3 exposure)
{
    return exposure.x / exp(clamp(color, exposure.y, exposure.z));
}

in highp vec2 vUV0;
void main()
{
    float newLum = Downscale2x2(RT, vUV0);
    newLum = expose(newLum, Exposure);
    float oldLum = texelFetch(Lum, ivec2(0, 0), 0).r;
    float lum = mix(newLum, oldLum, pow(0.25, timeSinceLast));
    FragColor.r = SafeHDR(lum);
}
