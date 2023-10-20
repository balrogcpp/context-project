// created by Andrey Vasiliev

#ifndef __VERSION__
#if defined(OGRE_GLSL)
#version 330 core
#define __VERSION__ 330
#elif defined(OGRE_GLSLES)
#version 300 es
#define __VERSION__ 300
#endif
#endif

#include "header.glsl"
#include "tonemap.glsl"

uniform sampler2D RT;
uniform sampler2D Lum;
uniform vec3 Exposure;
uniform float timeSinceLast;

float Downscale2x2(sampler2D tex, const vec2 uv, const vec2 tsize)
{
    float A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).x;
    float B = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).x;
    float C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).x;
    float D = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).x;

    float c1 = (A + B + C + D) * 0.25;

    return c1;
}

highp float expose(const highp float color, const highp vec3 exposure)
{
    return exposure.x / exp(clamp(color, exposure.y, exposure.z));
}

in highp vec2 vUV0;
void main()
{
    vec2 TexelSize0 = 1.0 / vec2(textureSize(RT, 0));
    float newLum = Downscale2x2(RT, vUV0, TexelSize0);
    newLum = expose(newLum, Exposure);
    float oldLum = texture2D(Lum, vec2(0.0, 0.0)).r;
    float lum = mix(newLum, oldLum, pow(0.25, timeSinceLast));

    FragColor.r = SafeHDR(lum);
}
