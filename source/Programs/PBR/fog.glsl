// created by Andrey Vasiliev

#ifndef FOG_GLSL
#define FOG_GLSL

vec3 ApplyFog(const vec3 color, float fog_param, const vec3 fog_color, float distance)
{
    float fogAmount = 1.0 - exp(-distance * fog_param);
    return mix(color, fog_color, fogAmount);
}

float pow8(float x)
{
    float x2 = x * x;
    float x4 = x2 * x2;
    return x4 * x4;
}

// https://iquilezles.org/articles/fog/
vec3 ApplyFog(const vec3 color, float fog_param, const vec3 fog_color, float distance, const vec3 rd, const vec3 lig, const vec3 camPos)
{
    float fogAmount = exp(-camPos.y * fog_param) * (1.0 - exp(-distance * rd.y * fog_param)) / rd.y;
    float sunAmount = max(dot(rd, lig), 0.0);
    vec3  fogColor  = mix(fog_color, vec3(1.0, 0.9, 0.7), pow8(sunAmount));
    return mix(color, fogColor, fogAmount);
}

#endif // FOG_GLSL
