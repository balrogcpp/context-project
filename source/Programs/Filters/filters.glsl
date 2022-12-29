// created by Andrey Vasiliev

#ifndef FILTERS_GLSL
#define FILTERS_GLSL


#include "srgb.glsl"


//----------------------------------------------------------------------------------------------------------------------
vec3 Linear(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  return texture2D(tex, uv).rgb;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 BoxFilter4(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  vec3 A = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0))).rgb;
  vec3 B = texture2D(tex, uv + (tsize * vec2(-1.0, 0.0))).rgb;
  vec3 C = texture2D(tex, uv + (tsize * vec2(0.0, -1.0))).rgb;
  vec3 D = texture2D(tex, uv + (tsize * vec2(0.0, 0.0))).rgb;

  vec3 color = (A + B + C + D) * 0.25;

  return color;
}


//----------------------------------------------------------------------------------------------------------------------
float BoxFilter4R(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  float A = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0))).r;
  float B = texture2D(tex, uv + (tsize * vec2(-1.0, 0.0))).r;
  float C = texture2D(tex, uv + (tsize * vec2(0.0, -1.0))).r;
  float D = texture2D(tex, uv + (tsize * vec2(0.0, 0.0))).r;

  float color = (A + B + C + D) * 0.25;

  return color;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 BoxFilter9(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  vec3 A = texture2D(tex, uv + (tsize * vec2(-2.0, -2.0))).rgb;
  vec3 B = texture2D(tex, uv + (tsize * vec2(-2.0, -1.0))).rgb;
  vec3 C = texture2D(tex, uv + (tsize * vec2(-2.0, 0.0))).rgb;
  vec3 D = texture2D(tex, uv + (tsize * vec2(-1.0, -2.0))).rgb;
  vec3 E = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0))).rgb;
  vec3 F = texture2D(tex, uv + (tsize * vec2(-1.0, 0.0))).rgb;
  vec3 G = texture2D(tex, uv + (tsize * vec2(0.0, -2.0))).rgb;
  vec3 H = texture2D(tex, uv + (tsize * vec2(0.0, -1.0))).rgb;
  vec3 I = texture2D(tex, uv + (tsize * vec2(0.0, 0.0))).rgb;

  vec3 color = (A + B + C) * 0.111111111111111;
  color += (D + E + F) * 0.111111111111111;
  color += (G + H + I) * 0.111111111111111;

  return color;
}


//----------------------------------------------------------------------------------------------------------------------
float BoxFilter9R(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  float A = texture2D(tex, uv + (tsize * vec2(-2.0, -2.0))).r;
  float B = texture2D(tex, uv + (tsize * vec2(-2.0, -1.0))).r;
  float C = texture2D(tex, uv + (tsize * vec2(-2.0, 0.0))).r;
  float D = texture2D(tex, uv + (tsize * vec2(-1.0, -2.0))).r;
  float E = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0))).r;
  float F = texture2D(tex, uv + (tsize * vec2(-1.0, 0.0))).r;
  float G = texture2D(tex, uv + (tsize * vec2(0.0, -2.0))).r;
  float H = texture2D(tex, uv + (tsize * vec2(0.0, -1.0))).r;
  float I = texture2D(tex, uv + (tsize * vec2(0.0, 0.0))).r;

  float color = (A + B + C) * 0.111111111111111;
  color += (D + E + F) * 0.111111111111111;
  color += (G + H + I) * 0.111111111111111;

  return color;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 BoxFilter16(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  vec3 A = texture2D(tex, uv + (tsize * vec2(-2.0, -2.0))).rgb;
  vec3 B = texture2D(tex, uv + (tsize * vec2(-2.0, -1.0))).rgb;
  vec3 C = texture2D(tex, uv + (tsize * vec2(-2.0, 0.0))).rgb;
  vec3 D = texture2D(tex, uv + (tsize * vec2(-2.0, 1.0))).rgb;
  vec3 E = texture2D(tex, uv + (tsize * vec2(-1.0, -2.0))).rgb;
  vec3 F = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0))).rgb;
  vec3 G = texture2D(tex, uv + (tsize * vec2(-1.0, 0.0))).rgb;
  vec3 H = texture2D(tex, uv + (tsize * vec2(-1.0, 1.0))).rgb;
  vec3 I = texture2D(tex, uv + (tsize * vec2(0.0, -2.0))).rgb;
  vec3 J = texture2D(tex, uv + (tsize * vec2(0.0, -1.0))).rgb;
  vec3 K = texture2D(tex, uv + (tsize * vec2(0.0, 0.0))).rgb;
  vec3 L = texture2D(tex, uv + (tsize * vec2(0.0, 1.0))).rgb;
  vec3 M = texture2D(tex, uv + (tsize * vec2(1.0, -2.0))).rgb;
  vec3 N = texture2D(tex, uv + (tsize * vec2(1.0, -1.0))).rgb;
  vec3 O = texture2D(tex, uv + (tsize * vec2(1.0, 0.0))).rgb;
  vec3 P = texture2D(tex, uv + (tsize * vec2(1.0, 1.0))).rgb;
  
  vec3 color = (A + B + C + D) * 0.0625;
  color += (E + F + G + H) * 0.0625;
  color += (I + J + K + L) * 0.0625;
  color += (M + N + O + P) * 0.0625;

  return color;
}


//----------------------------------------------------------------------------------------------------------------------
float BoxFilter16R(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  float A = texture2D(tex, uv + (tsize * vec2(-2.0, -2.0))).r;
  float B = texture2D(tex, uv + (tsize * vec2(-2.0, -1.0))).r;
  float C = texture2D(tex, uv + (tsize * vec2(-2.0, 0.0))).r;
  float D = texture2D(tex, uv + (tsize * vec2(-2.0, 1.0))).r;
  float E = texture2D(tex, uv + (tsize * vec2(-1.0, -2.0))).r;
  float F = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0))).r;
  float G = texture2D(tex, uv + (tsize * vec2(-1.0, 0.0))).r;
  float H = texture2D(tex, uv + (tsize * vec2(-1.0, 1.0))).r;
  float I = texture2D(tex, uv + (tsize * vec2(0.0, -2.0))).r;
  float J = texture2D(tex, uv + (tsize * vec2(0.0, -1.0))).r;
  float K = texture2D(tex, uv + (tsize * vec2(0.0, 0.0))).r;
  float L = texture2D(tex, uv + (tsize * vec2(0.0, 1.0))).r;
  float M = texture2D(tex, uv + (tsize * vec2(1.0, -2.0))).r;
  float N = texture2D(tex, uv + (tsize * vec2(1.0, -1.0))).r;
  float O = texture2D(tex, uv + (tsize * vec2(1.0, 0.0))).r;
  float P = texture2D(tex, uv + (tsize * vec2(1.0, 1.0))).r;

  float color = (A + B + C + D) * 0.0625;
  color += (E + F + G + H) * 0.0625;
  color += (I + J + K + L) * 0.0625;
  color += (M + N + O + P) * 0.0625;

  return color;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 Gauss5V(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  vec3 A = texture2D(tex, uv).rgb;
  vec3 B = texture2D(tex, uv + tsize * vec2(0.0, 1.3333333333333333)).rgb;
  vec3 C = texture2D(tex, uv - tsize * vec2(0.0, 1.3333333333333333)).rgb;

  vec3 color = A * 0.29411764705882354 + (B + C) * 0.35294117647058826;

  return color;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 Gauss5H(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  vec3 A = texture2D(tex, uv).rgb;
  vec3 B = texture2D(tex, uv + tsize * vec2(1.3333333333333333, 0.0)).rgb;
  vec3 C = texture2D(tex, uv - tsize * vec2(1.3333333333333333, 0.0)).rgb;

  vec3 color = A * 0.29411764705882354 + (B + C) * 0.35294117647058826;

  return color;
}


//----------------------------------------------------------------------------------------------------------------------
float Gauss5VR(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  float A = texture2D(tex, uv).r;
  float B = texture2D(tex, uv + tsize * vec2(0.0, 1.3333333333333333)).r;
  float C = texture2D(tex, uv - tsize * vec2(0.0, 1.3333333333333333)).r;

  float color = A * 0.29411764705882354 + (B + C) * 0.35294117647058826;

  return color;
}


//----------------------------------------------------------------------------------------------------------------------
float Gauss5HR(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  float A = texture2D(tex, uv).r;
  float B = texture2D(tex, uv + tsize * vec2(1.3333333333333333, 0.0)).r;
  float C = texture2D(tex, uv - tsize * vec2(1.3333333333333333, 0.0)).r;

  float color = A * 0.29411764705882354 + (B + C) * 0.35294117647058826;

  return color;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 Gauss9V(const sampler2D tex, const vec2 uv, const vec2 tsize)
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
vec3 Gauss9H(const sampler2D tex, const vec2 uv, const vec2 tsize)
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
float Gauss9VR(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  float A = texture2D(tex, uv).r;
  float B = texture2D(tex, uv + tsize * vec2(0.0, 1.3846153846)).r;
  float C = texture2D(tex, uv - tsize * vec2(0.0, 1.3846153846)).r;
  float D = texture2D(tex, uv + tsize * vec2(0.0, 3.2307692308)).r;
  float E = texture2D(tex, uv - tsize * vec2(0.0, 3.2307692308)).r;

  float color = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

  return color;
}


//----------------------------------------------------------------------------------------------------------------------
float Gauss9HR(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  float A = texture2D(tex, uv).r;
  float B = texture2D(tex, uv + tsize * vec2(1.3846153846, 0.0)).r;
  float C = texture2D(tex, uv - tsize * vec2(1.3846153846, 0.0)).r;
  float D = texture2D(tex, uv + tsize * vec2(3.2307692308, 0.0)).r;
  float E = texture2D(tex, uv - tsize * vec2(3.2307692308, 0.0)).r;

  float color = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

  return color;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 Gauss13V(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  vec3 A = texture2D(tex, uv).rgb;
  vec3 B = texture2D(tex, uv + tsize * vec2(0.0, 1.411764705882353)).rgb;
  vec3 C = texture2D(tex, uv - tsize * vec2(0.0, 1.411764705882353)).rgb;
  vec3 D = texture2D(tex, uv + tsize * vec2(0.0, 3.2941176470588234)).rgb;
  vec3 E = texture2D(tex, uv - tsize * vec2(0.0, 3.2941176470588234)).rgb;
  vec3 F = texture2D(tex, uv - tsize * vec2(0.0, 5.176470588235294)).rgb;
  vec3 G = texture2D(tex, uv - tsize * vec2(0.0, 5.176470588235294)).rgb;

  vec3 color = A * 0.1964825501511404 + (B + C) * 0.2969069646728344 + (D + E) * 0.09447039785044732 + (F + G) * 0.010381362401148057;

  return color;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 Gauss13H(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  vec3 A = texture2D(tex, uv).rgb;
  vec3 B = texture2D(tex, uv + tsize * vec2(1.411764705882353, 0.0)).rgb;
  vec3 C = texture2D(tex, uv - tsize * vec2(1.411764705882353, 0.0)).rgb;
  vec3 D = texture2D(tex, uv + tsize * vec2(3.2941176470588234, 0.0)).rgb;
  vec3 E = texture2D(tex, uv - tsize * vec2(3.2941176470588234, 0.0)).rgb;
  vec3 F = texture2D(tex, uv - tsize * vec2(5.176470588235294, 0.0)).rgb;
  vec3 G = texture2D(tex, uv - tsize * vec2(5.176470588235294, 0.0)).rgb;

  vec3 color = A * 0.1964825501511404 + (B + C) * 0.2969069646728344 + (D + E) * 0.09447039785044732 + (F + G) * 0.010381362401148057;

  return color;
}


//----------------------------------------------------------------------------------------------------------------------
float Gauss13VR(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  float A = texture2D(tex, uv).r;
  float B = texture2D(tex, uv + tsize * vec2(0.0, 1.411764705882353)).r;
  float C = texture2D(tex, uv - tsize * vec2(0.0, 1.411764705882353)).r;
  float D = texture2D(tex, uv + tsize * vec2(0.0, 3.2941176470588234)).r;
  float E = texture2D(tex, uv - tsize * vec2(0.0, 3.2941176470588234)).r;
  float F = texture2D(tex, uv - tsize * vec2(0.0, 5.176470588235294)).r;
  float G = texture2D(tex, uv - tsize * vec2(0.0, 5.176470588235294)).r;

  float color = A * 0.1964825501511404 + (B + C) * 0.2969069646728344 + (D + E) * 0.09447039785044732 + (F + G) * 0.010381362401148057;

  return color;
}


//----------------------------------------------------------------------------------------------------------------------
float Gauss13HR(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  float A = texture2D(tex, uv).r;
  float B = texture2D(tex, uv + tsize * vec2(1.411764705882353, 0.0)).r;
  float C = texture2D(tex, uv - tsize * vec2(1.411764705882353, 0.0)).r;
  float D = texture2D(tex, uv + tsize * vec2(3.2941176470588234, 0.0)).r;
  float E = texture2D(tex, uv - tsize * vec2(3.2941176470588234, 0.0)).r;
  float F = texture2D(tex, uv - tsize * vec2(5.176470588235294, 0.0)).r;
  float G = texture2D(tex, uv - tsize * vec2(5.176470588235294, 0.0)).r;

  float color = A * 0.1964825501511404 + (B + C) * 0.2969069646728344 + (D + E) * 0.09447039785044732 + (F + G) * 0.010381362401148057;

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
//  https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L57
float Upscale3x3R(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  float A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).r;
  float B = texture2D(tex, uv + tsize * vec2(0.0, -1.0)).r;
  float C = texture2D(tex, uv + tsize * vec2(1.0, -1.0)).r;
  float D = texture2D(tex, uv + tsize * vec2(-1.0, 0.0)).r;
  float E = texture2D(tex, uv + tsize                  ).r;
  float F = texture2D(tex, uv + tsize * vec2(1.0, 0.0)).r;
  float G = texture2D(tex, uv + tsize * vec2(-1.0, 1.0)).r;
  float H = texture2D(tex, uv + tsize * vec2(0.0,  1.0)).r;
  float I = texture2D(tex, uv + tsize * vec2(1.0, 1.0)).r;

  float color = E * 0.25;
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
float Upscale2x2R(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  float A = texture2D(tex, uv + (tsize * vec2(-0.5, -0.5))).r;
  float B = texture2D(tex, uv + (tsize * vec2(-0.5, 0.5))).r;
  float C = texture2D(tex, uv + (tsize * vec2(-0.5, 0.5))).r;
  float D = texture2D(tex, uv + (tsize * vec2(0.5, -0.5))).r;

  float color = (A + B + C + D) * 0.25; // 1/4

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
float Downscale2x2R(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  float A = texture2D(tex, uv + (tsize * vec2(-0.5, -0.5))).r;
  float B = texture2D(tex, uv + (tsize * vec2(-0.5, 0.5))).r;
  float C = texture2D(tex, uv + (tsize * vec2(-0.5, 0.5))).r;
  float D = texture2D(tex, uv + (tsize * vec2(0.5, -0.5))).r;

  float color = (A + B + C + D) * 0.25; // 1/4

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
float Downscale4x4R(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  float A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).r;
  float B = texture2D(tex, uv + tsize * vec2( 0.0, -1.0)).r;
  float C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).r;
  float D = texture2D(tex, uv + tsize * vec2(-0.5, -0.5)).r;
  float E = texture2D(tex, uv + tsize * vec2( 0.5, -0.5)).r;
  float F = texture2D(tex, uv + tsize * vec2(-1.0,  0.0)).r;
  float G = texture2D(tex, uv                           ).r;
  float H = texture2D(tex, uv + tsize * vec2( 1.0,  0.0)).r;
  float I = texture2D(tex, uv + tsize * vec2(-0.5,  0.5)).r;
  float J = texture2D(tex, uv + tsize * vec2( 0.5,  0.5)).r;
  float K = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).r;
  float L = texture2D(tex, uv + tsize * vec2( 0.0,  1.0)).r;
  float M = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).r;

  float color = (D + E + I + J) * 0.125;
  color += (A + B + G + F) * 0.03125;
  color += (B + C + H + G) * 0.03125;
  color += (F + G + L + K) * 0.03125;
  color += (G + H + M + L) * 0.03125;

  return color;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 Downscale13T(const sampler2D tex, const vec2 uv, const vec2 tsize)
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

  vec3 c1 = (D + E + I + J); c1 /= (1.0 + luminance(LINEARtoSRGB(c1))); c1 *= 0.125;
  vec3 c2 = (A + B + G + F); c2 /= (1.0 + luminance(LINEARtoSRGB(c2))); c2 *= 0.03125;
  vec3 c3 = (B + C + H + G); c3 /= (1.0 + luminance(LINEARtoSRGB(c3))); c3 *= 0.03125;
  vec3 c4 = (F + G + L + K); c4 /= (1.0 + luminance(LINEARtoSRGB(c4))); c4 *= 0.03125;
  vec3 c5 = (G + H + M + L); c5 /= (1.0 + luminance(LINEARtoSRGB(c5))); c5 *= 0.03125;

  return c1 + c2 + c3 + c4 + c5;
}


//----------------------------------------------------------------------------------------------------------------------
vec3 Downscale13LUM(const sampler2D tex, const vec2 uv, const vec2 tsize)
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

  vec3 c1 = (D + E + I + J); c1 /= (1.0 + luminance(c1)); c1 *= 0.125;
  vec3 c2 = (A + B + G + F); c2 /= (1.0 + luminance(c2)); c2 *= 0.03125;
  vec3 c3 = (B + C + H + G); c3 /= (1.0 + luminance(c3)); c3 *= 0.03125;
  vec3 c4 = (F + G + L + K); c4 /= (1.0 + luminance(c4)); c4 *= 0.03125;
  vec3 c5 = (G + H + M + L); c5 /= (1.0 + luminance(c5)); c5 *= 0.03125;

  return c1 + c2 + c3 + c4 + c5;
}



//----------------------------------------------------------------------------------------------------------------------
vec3 FXAA(const sampler2D tex, const vec2 uv, const vec2 tsize, const float strength)
{
  #define reducemul 0.125 // 1/8
  #define reducemin 0.0078125 // 1/128
  #define gray vec3(0.299, 0.587, 0.114)

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
