// This source file is part of "glue project". Created by Andrey Vasiliev

vec3 ApplyFog(vec3 color, vec4 fog_params, vec3 fog_color, float depth) {
    float fog_value = 1.0 - clamp(1.0/exp(depth * fog_params.x), 0.0, 1.0);
    return mix(color, fog_color, fog_value);
}

vec4 ApplyFog(vec4 color, vec4 fog_params, vec3 fog_color, float depth) {
    float fog_value = 1.0 - clamp(1.0/exp(depth * fog_params.x), 0.0, 1.0);
    color.rgb = mix(color.rgb, fog_color, fog_value);
    return color;
}
