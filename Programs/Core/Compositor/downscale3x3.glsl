// This source file is part of Glue Engine. Created by Andrey Vasiliev

#ifndef DOWNSCALE3X3_GLSL
#define DOWNSCALE3X3_GLSL

//----------------------------------------------------------------------------------------------------------------------
vec3 Downscale3x3(const sampler2D sampler, const vec2 uv, const vec2 tsize) {
  vec3 color = vec3(0.0);

  const vec2 Poisson0 = vec2(0.0, 0.0);
  const vec2 Poisson1 = vec2(0.0, 1.0);
  const vec2 Poisson2 = vec2(0.0, -1.0);
  const vec2 Poisson3 = vec2(-0.866025, 0.5);
  const vec2 Poisson4 = vec2(-0.866025, -0.5);
  const vec2 Poisson5 = vec2(0.866025, 0.5);
  const vec2 Poisson6 = vec2(0.866025, -0.5);
  const float InvNumSamples = 0.1428571428571429; //<- 1.0 / number of Poisson samples

  color += texture2D(sampler, uv + (Poisson0 * tsize)).rgb;
  color += texture2D(sampler, uv + (Poisson1 * tsize)).rgb;
  color += texture2D(sampler, uv + (Poisson2 * tsize)).rgb;
  color += texture2D(sampler, uv + (Poisson3 * tsize)).rgb;
  color += texture2D(sampler, uv + (Poisson4 * tsize)).rgb;
  color += texture2D(sampler, uv + (Poisson5 * tsize)).rgb;
  color += texture2D(sampler, uv + (Poisson6 * tsize)).rgb;
  color *= InvNumSamples;

  return color;
}

#endif // DOWNSCALE3X3_GLSL
