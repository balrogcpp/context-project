// This source file is part of "glue project". Created by Andrey Vasiliev

#ifndef GL_ES

vec3 ApplyFog(vec3 color, vec4 fog_params, vec3 fog_color, float depth) {
    float exponent = depth * fog_params.x;
    float fog_value = 1.0 - clamp(1.0 / exp(exponent), 0.0, 1.0);
    return mix(color, fog_color, fog_value);
}

vec4 ApplyFog(vec4 color, vec4 fog_params, vec3 fog_color, float depth) {
    float exponent = depth * fog_params.x;
    float fog_value = 1.0 - clamp(1.0 / exp(exponent), 0.0, 1.0);
    color.rgb = mix(color.rgb, fog_color, fog_value);
    return color;
}


#else

float Exp(float x) {
    return 1.0 + x + x * x / 2.0;
}

vec3 ApplyFog(vec3 color, vec4 fog_params, vec3 fog_color, float depth) {
    float exponent = depth * fog_params.x;
    float fog_value = 1.0 - clamp(1.0 / Exp(exponent), 0.0, 1.0);
    return mix(color, fog_color, fog_value);
}

vec4 ApplyFog(vec4 color, vec4 fog_params, vec3 fog_color, float depth) {
    float exponent = depth * fog_params.x;
    float fog_value = 1.0 - clamp(1.0 / Exp(exponent), 0.0, 1.0);
    color.rgb = mix(color.rgb, fog_color, fog_value);
    return color;
}

#endif
