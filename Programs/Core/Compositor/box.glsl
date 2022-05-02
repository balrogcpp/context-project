// This source file is part of Glue Engine. Created by Andrey Vasiliev

#ifndef BOX_GLSL
#define BOX_GLSL

//----------------------------------------------------------------------------------------------------------------------
vec3 BoxFilter(const sampler2D sampler, const vec2 uv, const vec2 tsize)
{
  vec3 color = vec3(0.0);
  for (int x = -2; x < 1; x++)
  for (int y = -2; y < 1; y++)
    color += texture2D(sampler, uv + vec2(float(x), float(y)) * tsize).rgb;
  color *= 0.11111111111111111111; // 1/9

  return color;
}

//----------------------------------------------------------------------------------------------------------------------
float BoxFilterR(const sampler2D sampler, const vec2 uv, const vec2 tsize)
{
  float color = 0.0;
  for (int x = -2; x < 1; x++)
  for (int y = -2; y < 1; y++)
    color += texture2D(sampler, uv + vec2(float(x), float(y)) * tsize).r;
  color *= 0.11111111111111111111; // 1/9

  return color;
}

#endif // BOX_GLSL
