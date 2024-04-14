// created by Andrey Vasiliev

#ifndef FOG_GLSL
#define FOG_GLSL

vec3 ApplyFog(const vec3 color, float fogParam, const vec3 fogColor, highp float dist)
{
    float fogAmount = 1.0 - exp(-dist * fogParam);
    return mix(color, fogColor, fogAmount);
}

float pow8(float x)
{
    float x2 = x * x;
    float x4 = x2 * x2;
    return x4 * x4;
}

// https://iquilezles.org/articles/fog/
vec3 ApplyFog(const vec3 color, float fogParam, vec3 fogColor, const highp float dist, const vec3 camDir, const vec3 lightDir, const highp vec3 camPos)
{
    float a = fogParam;
    float b = fogParam;
    float camHeight = camDir.y;
    if (abs(camHeight) < 0.001) camHeight = 0.001 * sign(camHeight);
    float fogAmount = (a / b) * exp(-camHeight * b) * (1.0 - exp(-dist * camHeight * b)) / camHeight;

    float sunAmount = max(dot(camDir, -lightDir), 0.0);
    //const vec3 sunColor = vec3(1.0, 0.9, 0.7);
    const vec3 sunColor = vec3(1.0, 1.0, 1.0);
    fogColor  = mix(fogColor, sunColor, pow8(sunAmount));
    fogAmount = saturate(fogAmount);
    return mix(color, fogColor, fogAmount);
}

#endif // FOG_GLSL
