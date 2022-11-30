// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif

#include "header.frag"
#include "filters.glsl"


in vec2 vUV0;
uniform sampler2D uSampler;
uniform vec2 TexelSize0;
uniform vec2 TexSize0;
uniform vec4 ViewportSize;


//----------------------------------------------------------------------------------------------------------------------
vec3 SampleGhosts(const sampler2D tex, const vec2 _uv)
{
    vec2 uv = vec2(1.0) - _uv;
    vec2 ghostVec = (vec2(0.5) - uv) * 0.5;
    vec3 ret = vec3(0.0);

    for (float i = 1.0; i < 2.0; i += 1.0)
    {
        vec2 suv = fract(uv + ghostVec * vec2(i));
        vec3 s = texture2D(tex, suv).rgb;

        float d = length(vec2(0.5) - suv) / length(vec2(0.5));
        float weight = pow(1.0 - d, 10.0);

        ret += s * weight;
    }

    return ret;
}


vec3 SampleHalo(const sampler2D tex, const vec2 _uv, const float radius)
{
   vec2 uv = vec2(1.0) - _uv;
   vec2 ghostVec = (vec2(0.5) - uv) * 0.5;
   vec2 haloVec = normalize(ghostVec) * radius;

  vec2 uvw = normalize(uv * TexSize0.xy);
   float weight = length(vec2(0.5) - fract(uvw + haloVec)) / length(vec2(0.5));
   weight = pow(1.0 - weight, 5.0);
   vec3 s = texture2D(tex, uv + haloVec).rgb;

  return s * weight;
}


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 color = Downscale4x4(uSampler, vUV0, TexelSize0);
  color += SampleGhosts(uSampler, vUV0);
  color += SampleHalo(uSampler, vUV0, 0.5);
  FragColor.rgb = color;
}
