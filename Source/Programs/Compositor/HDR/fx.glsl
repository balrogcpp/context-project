// created by Andrey Vasiliev
//? #version 400


#include "grain.glsl"
#include "lut.glsl"
#include "vignette.glsl"

uniform vec4 VignetteParams;
uniform vec3 VignetteColor;
uniform float Time;


vec3 applyPostEffects(in vec3 color, const vec2 uv, const sampler2D lookupTable) {
    //color = lookup(color, lookupTable);
    color = grainHigh(color, uv, Time);
    color = vignette(color, uv, VignetteParams, VignetteColor);

    return color;
}
