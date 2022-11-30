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


//----------------------------------------------------------------------------------------------------------------------
vec3 SampleGhosts(const sampler2D tex, const vec2 _uv, const vec2 tsize)
{
    vec2 uv = vec2(1.0) - _uv;
    vec3 ret = vec3(0.0);
    vec2 ghostVec = (vec2(0.5) - uv) * 0.5;

    for (float i = 1.0; i < 2.0; i += 1.0)
    {
        vec2 suv = fract(uv + ghostVec * vec2(i));
        vec3 s = texture2D(tex, suv).rgb;

        float d = distance(suv, vec2(0.5));
        //float weight = 1.0 - smoothstep(0.0, 0.75, d);
        //float weight = texture2D(uGhostColorGradient, vec2(d, 0.5), 0.0).r;
        float weight = pow(1.0 - d, 10.0);

        ret += s * weight;
    }

    //ret *= texture2D(uGhostColorGradient, vec2(distance(uv, vec2(0.5)), 0.5), 0.0).rgb;

    return ret;
}


vec3 SampleHelo(const sampler2D tex, const vec2 _uv, const vec2 tsize)
{
   vec2 uv = vec2(1.0) - _uv;
   vec2 haloVec = normalize(vec2(0.5) - uv);

   float weight = length(vec2(0.5) - fract(uv + haloVec)) / length(vec2(0.5));
   weight = pow(1.0 - weight, 5.0);
   vec3 s = texture2D(tex, uv + haloVec).rgb;

  return s * weight;
}


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 color = Downscale4x4(uSampler, vUV0, TexelSize0);
  color += SampleGhosts(uSampler, vUV0, TexelSize0);
  color += SampleHelo(uSampler, vUV0, TexelSize0);
  FragColor.rgb = color;
}
