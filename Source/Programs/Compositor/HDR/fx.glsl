// created by Andrey Vasiliev
//? #version 400

#include "grain.glsl"
#include "lut.glsl"
#include "vignette.glsl"


vec3 applyPostEffects(in vec3 color, const vec2 uv, const float time, const sampler2D lookupTable) {
    //color = lookup(color, lookupTable);
    color = grainHigh(color, uv, time);
    color = vignette(color, uv, vec4(0.0, 0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0));

    return color;
}
