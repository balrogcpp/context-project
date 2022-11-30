// created by Andrey Vasiliev

#ifndef FILTERS_GLSL
#define FILTERS_GLSL

//----------------------------------------------------------------------------------------------------------------------
vec3 Linear(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  return texture2D(tex, uv).rgb;
}

//----------------------------------------------------------------------------------------------------------------------
vec3 BoxFilter4(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  vec3 color = vec3(0.0);
  for (int x = -1; x < 1; x++)
  for (int y = -1; y < 1; y++)
    color += texture2D(tex, uv + vec2(float(x), float(y)) * tsize).rgb;
  color *= 0.25; // 1/4

  return color;
}

//----------------------------------------------------------------------------------------------------------------------
float BoxFilter4R(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  float color = 0.0;
  for (int x = -1; x < 1; x++)
  for (int y = -1; y < 1; y++)
    color += texture2D(tex, uv + vec2(float(x), float(y)) * tsize).r;
  color *= 0.25; // 1/4

  return color;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 BoxFilter9(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  vec3 color = vec3(0.0);
  for (int x = -2; x < 1; x++)
  for (int y = -2; y < 1; y++)
    color += texture2D(tex, uv + vec2(float(x), float(y)) * tsize).rgb;
  color *= 0.11111111111111111111; // 1/9

  return color;
}

//----------------------------------------------------------------------------------------------------------------------
float BoxFilter9R(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  float color = 0.0;
  for (int x = -2; x < 1; x++)
  for (int y = -2; y < 1; y++)
    color += texture2D(tex, uv + vec2(float(x), float(y)) * tsize).r;
  color *= 0.11111111111111111111; // 1/9

  return color;
}

//----------------------------------------------------------------------------------------------------------------------
vec3 BoxFilter16(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  vec3 color = vec3(0.0);
  for (int x = -2; x < 2; x++)
  for (int y = -2; y < 2; y++)
    color += texture2D(tex, uv + vec2(float(x), float(y)) * tsize).rgb;
  color *= 0.0625; // 1/16

  return color;
}

//----------------------------------------------------------------------------------------------------------------------
float BoxFilter16R(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  float color = 0.0;
  for (int x = -2; x < 2; x++)
  for (int y = -2; y < 2; y++)
    color += texture2D(tex, uv + vec2(float(x), float(y)) * tsize).r;
  color *= 0.0625; // 1/16

  return color;
}

//----------------------------------------------------------------------------------------------------------------------
vec3 GaussV(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  vec3 A = texture2D(tex, uv).rgb;
  vec3 B = texture2D(tex, uv + tsize * vec2(0.0, 1.3846153846)).rgb;
  vec3 C = texture2D(tex, uv - tsize * vec2(0.0, 1.3846153846)).rgb;
  vec3 D = texture2D(tex, uv + tsize * vec2(0.0, 3.2307692308)).rgb;
  vec3 E = texture2D(tex, uv - tsize * vec2(0.0, 3.2307692308)).rgb;

  vec3 color = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

  return color;
}

//----------------------------------------------------------------------------------------------------------------------
vec3 GaussH(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  vec3 A = texture2D(tex, uv).rgb;
  vec3 B = texture2D(tex, uv + tsize * vec2(1.3846153846, 0.0)).rgb;
  vec3 C = texture2D(tex, uv - tsize * vec2(1.3846153846, 0.0)).rgb;
  vec3 D = texture2D(tex, uv + tsize * vec2(3.2307692308, 0.0)).rgb;
  vec3 E = texture2D(tex, uv - tsize * vec2(3.2307692308, 0.0)).rgb;

  vec3 color = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

  return color;
}

//----------------------------------------------------------------------------------------------------------------------
//  https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L57
vec3 Upscale3x3(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  vec3 A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).rgb;
  vec3 B = texture2D(tex, uv + tsize * vec2(0.0, -1.0)).rgb;
  vec3 C = texture2D(tex, uv + tsize * vec2(1.0, -1.0)).rgb;
  vec3 D = texture2D(tex, uv + tsize * vec2(-1.0, 0.0)).rgb;
  vec3 E = texture2D(tex, uv + tsize                  ).rgb;
  vec3 F = texture2D(tex, uv + tsize * vec2(1.0, 0.0)).rgb;
  vec3 G = texture2D(tex, uv + tsize * vec2(-1.0, 1.0)).rgb;
  vec3 H = texture2D(tex, uv + tsize * vec2(0.0,  1.0)).rgb;
  vec3 I = texture2D(tex, uv + tsize * vec2(1.0, 1.0)).rgb;

  vec3 color = E * 0.25;
  color += (B + D + F + H) * 0.125;
  color += (A + C + G + I) * 0.0625;

  return color;
}

//----------------------------------------------------------------------------------------------------------------------
vec3 Upscale2x2(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  vec3 A = texture2D(tex, uv + (tsize * vec2(-0.5, -0.5))).rgb;
  vec3 B = texture2D(tex, uv + (tsize * vec2(-0.5, 0.5))).rgb;
  vec3 C = texture2D(tex, uv + (tsize * vec2(-0.5, 0.5))).rgb;
  vec3 D = texture2D(tex, uv + (tsize * vec2(0.5, -0.5))).rgb;

  vec3 color = (A + B + C + D) * 0.25; // 1/4

  return color;
}

//----------------------------------------------------------------------------------------------------------------------
vec3 Downscale2x2(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  vec3 A = texture2D(tex, uv + (tsize * vec2(-0.5, -0.5))).rgb;
  vec3 B = texture2D(tex, uv + (tsize * vec2(-0.5, 0.5))).rgb;
  vec3 C = texture2D(tex, uv + (tsize * vec2(-0.5, 0.5))).rgb;
  vec3 D = texture2D(tex, uv + (tsize * vec2(0.5, -0.5))).rgb;

  vec3 color = (A + B + C + D) * 0.25; // 1/4

  return color;
}

//----------------------------------------------------------------------------------------------------------------------
//  https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L15
vec3 Downscale4x4(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  vec3 A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).rgb;
  vec3 B = texture2D(tex, uv + tsize * vec2( 0.0, -1.0)).rgb;
  vec3 C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).rgb;
  vec3 D = texture2D(tex, uv + tsize * vec2(-0.5, -0.5)).rgb;
  vec3 E = texture2D(tex, uv + tsize * vec2( 0.5, -0.5)).rgb;
  vec3 F = texture2D(tex, uv + tsize * vec2(-1.0,  0.0)).rgb;
  vec3 G = texture2D(tex, uv                           ).rgb;
  vec3 H = texture2D(tex, uv + tsize * vec2( 1.0,  0.0)).rgb;
  vec3 I = texture2D(tex, uv + tsize * vec2(-0.5,  0.5)).rgb;
  vec3 J = texture2D(tex, uv + tsize * vec2( 0.5,  0.5)).rgb;
  vec3 K = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).rgb;
  vec3 L = texture2D(tex, uv + tsize * vec2( 0.0,  1.0)).rgb;
  vec3 M = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).rgb;

  vec3 color = (D + E + I + J) * 0.125;
  color += (A + B + G + F) * 0.03125;
  color += (B + C + H + G) * 0.03125;
  color += (F + G + L + K) * 0.03125;
  color += (G + H + M + L) * 0.03125;

  return color;
}

//----------------------------------------------------------------------------------------------------------------------
vec3 FXAA(const sampler2D tex, const vec2 uv, const vec2 tsize, const float strength)
{
  const float reducemul = 0.125; // 1/8
  const float reducemin = 0.0078125; // 1/128
  const vec3 gray = vec3(0.299, 0.587, 0.114);

  vec3 basecol = texture2D(tex, uv).rgb;
  vec3 baseNW = texture2D(tex, uv - tsize).rgb;
  vec3 baseNE = texture2D(tex, uv + vec2(tsize.x, -tsize.y)).rgb;
  vec3 baseSW = texture2D(tex, uv + vec2(-tsize.x, tsize.y)).rgb;
  vec3 baseSE = texture2D(tex, uv + tsize).rgb;
  float monocol = dot(basecol, gray);
  float monoNW = dot(baseNW, gray);
  float monoNE = dot(baseNE, gray);
  float monoSW = dot(baseSW, gray);
  float monoSE = dot(baseSE, gray);
  float monomin = min(monocol, min(min(monoNW, monoNE), min(monoSW, monoSE)));
  float monomax = max(monocol, max(max(monoNW, monoNE), max(monoSW, monoSE)));
  vec2 dir = vec2(-((monoNW + monoNE) - (monoSW + monoSE)), ((monoNW + monoSW) - (monoNE + monoSE)));
  float dirreduce = max((monoNW + monoNE + monoSW + monoSE) * reducemul * 0.25, reducemin);
  float dirmin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirreduce);
  dir = min(vec2(strength), max(vec2(-strength), dir * dirmin)) * tsize;

  vec3 resultA = 0.5 * (texture2D(tex, uv + dir * -0.166667).rgb + texture2D(tex, uv + dir * 0.166667).rgb);
  vec3 resultB = resultA * 0.5 + 0.25 * (texture2D(tex, uv + dir * -0.5).rgb + texture2D(tex, uv + dir * 0.5).rgb);
  float monoB = dot(resultB, gray);

  if(monoB < monomin || monoB > monomax)
    return resultA;
  else
    return resultB;
}

#endif // FILTERS_GLSL
