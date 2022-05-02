// This source file is part of Glue Engine. Created by Andrey Vasiliev

#ifndef DOWNSCALE2X2_GLSL
#define DOWNSCALE2X2_GLSL

//----------------------------------------------------------------------------------------------------------------------
vec3 Downscale2x2(const sampler2D sampler, const vec2 uv, const vec2 tsize) {
  const vec2 offsets0 = vec2(-0.5, -0.5);
  const vec2 offsets1 = vec2(-0.5, 0.5);
  const vec2 offsets2 = vec2(-0.5, 0.5);
  const vec2 offsets3 = vec2(0.5, -0.5);

  vec3 color = vec3(0.0);

  color += texture2D(sampler, uv + (offsets0 * tsize)).rgb;
  color += texture2D(sampler, uv + (offsets1 * tsize)).rgb;
  color += texture2D(sampler, uv + (offsets2 * tsize)).rgb;
  color += texture2D(sampler, uv + (offsets3 * tsize)).rgb;
  color *= 0.25; // 1/4

  return color;
}

#endif // DOWNSCALE2X2_GLSL
