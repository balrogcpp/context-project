// created by Andrey Vasiliev

#ifndef FOG_GLSL
#define FOG_GLSL

mediump vec3 ApplyFogLinear(const mediump vec3 color, const mediump vec4 fog_params, const mediump vec3 fog_color, const mediump float depth)
{
    float fog_factor = clamp((fog_params.z - depth) * fog_params.w, 0.0, 1.0);
    return mix(color, fog_color, fog_factor);
}

mediump vec3 ApplyFogExp(const mediump vec3 color, const mediump vec4 fog_params, const mediump vec3 fog_color, const float depth)
{
    float x = depth * fog_params.x;
    float fog_factor = 1.0 - clamp(1.0 / exp(x), 0.0, 1.0);
    return mix(color, fog_color, fog_factor);
}

mediump vec3 ApplyFogExp2(const mediump vec3 color, const mediump vec4 fog_params, const mediump vec3 fog_color, const mediump float depth)
{
    float x = depth * fog_params.x;
    float fog_factor = 1.0 - clamp(1.0 / exp(x * x), 0.0, 1.0);
    return mix(color, fog_color, fog_factor);
}

mediump vec3 ApplyFog(const mediump vec3 color, const mediump vec4 fog_params, const mediump vec3 fog_color, const mediump float depth)
{
    float x = depth * fog_params.x;
    float fog_factor = 1.0 - clamp(1.0 / exp(x), 0.0, 1.0);
    return mix(color, fog_color, fog_factor);
}

#endif // FOG_GLSL
