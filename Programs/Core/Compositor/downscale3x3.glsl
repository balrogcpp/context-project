// This source file is part of Glue Engine. Created by Andrey Vasiliev

#ifndef DOWNSCALE3X3_GLSL
#define DOWNSCALE3X3_GLSL

//----------------------------------------------------------------------------------------------------------------------
vec3 Downscale3x3(const sampler2D sampler, const vec2 uv, const vec2 tsize) {
  vec3 A = texture2D(sampler, uv + tsize * vec2(-1.0, -1.0)).rgb;
  vec3 B = texture2D(sampler, uv + tsize * vec2( 0.0, -1.0)).rgb;
  vec3 C = texture2D(sampler, uv + tsize * vec2( 1.0, -1.0)).rgb;
  vec3 D = texture2D(sampler, uv + tsize * vec2(-0.5, -0.5)).rgb;
  vec3 E = texture2D(sampler, uv + tsize * vec2( 0.5, -0.5)).rgb;
  vec3 F = texture2D(sampler, uv + tsize * vec2(-1.0,  0.0)).rgb;
  vec3 G = texture2D(sampler, uv                           ).rgb;
  vec3 H = texture2D(sampler, uv + tsize * vec2( 1.0,  0.0)).rgb;
  vec3 I = texture2D(sampler, uv + tsize * vec2(-0.5,  0.5)).rgb;
  vec3 J = texture2D(sampler, uv + tsize * vec2( 0.5,  0.5)).rgb;
  vec3 K = texture2D(sampler, uv + tsize * vec2(-1.0,  1.0)).rgb;
  vec3 L = texture2D(sampler, uv + tsize * vec2( 0.0,  1.0)).rgb;
  vec3 M = texture2D(sampler, uv + tsize * vec2( 1.0,  1.0)).rgb;

  vec3 color = (D + E + I + J) * 0.125;
  color += (A + B + G + F) * 0.03125;
  color += (B + C + H + G) * 0.03125;
  color += (F + G + L + K) * 0.03125;
  color += (G + H + M + L) * 0.03125;

  return color;
}

#endif // DOWNSCALE3X3_GLSL
