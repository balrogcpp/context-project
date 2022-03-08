// This source file is part of Glue Engine. Created by Andrey Vasiliev

#ifndef FOG_GLSL
#define FOG_GLSL

//----------------------------------------------------------------------------------------------------------------------
float EXP (float x)
{
    //return 1.0 + x + 0.5*x*x;
    return exp(x);
}

//----------------------------------------------------------------------------------------------------------------------
vec3 EXP (vec3 x)
{
    //return 1.0 + x + vec3(0.5)*x*x;
    return exp(x);
}

//----------------------------------------------------------------------------------------------------------------------
vec3 ApplyFog(vec3 color, vec4 fog_params, vec3 fog_color, float depth)
{
    float fog_value = 1.0 - clamp(1.0/EXP(depth * fog_params.x), 0.0, 1.0);
    return mix(color, fog_color, fog_value);
}

//----------------------------------------------------------------------------------------------------------------------
vec4 ApplyFog(vec4 color, vec4 fog_params, vec3 fog_color, float depth)
{
    float fog_value = 1.0 - clamp(1.0/EXP(depth * fog_params.x), 0.0, 1.0);
    color.rgb = mix(color.rgb, fog_color, fog_value);
    return color;
}

#endif // FOG_GLSL
