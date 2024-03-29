// This source file is part of Glue Engine. Created by Andrey Vasiliev

#ifndef GL_ES
#version 120
#define VERSION 120
#else
#version 100
#define VERSION 100
#endif

#include "header.frag"
#include "filters.glsl"

in vec2 vUV0;
uniform float uEnable;
uniform sampler2D uSampler;
uniform vec2 TexelSize;

//----------------------------------------------------------------------------------------------------------------------
vec3 Ghosts()
{
    vec2 uv = vec2(1.0) - vUV0; // flip the texture coordinates
    vec3 ret = vec3(0.0);
    vec2 ghostVec = (vec2(0.5) - uv) * 0.5;
    for (int i = 1; i < 2; i++)
    {
        vec2 suv = fract(uv + ghostVec * vec2(float(i)));
        float d = distance(suv, vec2(0.5));
        float weight = 1.0 - smoothstep(0.0, 0.75, d); // reduce contributions from samples at the screen edge
        //vec3 s = texture2D(uSampler, uv).rgb;
        vec3 s = Downscale3x3(uSampler, uv, TexelSize);
        ret += s * weight;
    }

    // sample halo:
    vec2 haloVec = normalize(ghostVec) * 50.0;
    float weight = length(vec2(0.5) - fract(vUV0 + haloVec)) / length(vec2(0.5));
    weight = pow(1.0 - weight, 5.0);
    ret += texture2D(uSampler, vUV0 + haloVec).rgb * weight;

    return ret;
}

//----------------------------------------------------------------------------------------------------------------------
void main()
{
    if (uEnable <= 0.0) discard;
    vec3 color = Downscale3x3(uSampler, vUV0, TexelSize);
    color += Ghosts();
    FragColor.rgb = color;
}
