//MIT License
//
//Copyright (c) 2021 Andrei Vasilev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#ifndef IBL_GLSL
#define IBL_GLSL

// Calculation of the lighting contribution from an optional Image Based Light source.
// Precomputed Environment Maps are required uniform inputs and are computed as outlined in [1].
// See our README.md on Environment Maps [3] for additional discussion.
//----------------------------------------------------------------------------------------------------------------------
vec3 GetIBLContribution(PBRInfo pbrInputs, vec3 n, vec3 reflection)
{
  float mipCount = 9.0;// resolution of 512x512
  float lod = (pbrInputs.perceptualRoughness * mipCount);
  // retrieve a scale and bias to F0. See [1], Figure 3
  vec3 brdf = SRGBtoLINEAR(texture2D(ubrdfLUT, vec2(pbrInputs.NdotV, 1.0 - pbrInputs.perceptualRoughness))).rgb;
  vec3 diffuseLight = SRGBtoLINEAR(textureCube(uDiffuseEnvSampler, n)).rgb;

#ifdef USE_TEX_LOD
  vec3 specularLight = SRGBtoLINEAR(textureCubeLod(uSpecularEnvSampler, reflection, lod)).rgb;
#else
  vec3 specularLight = SRGBtoLINEAR(textureCube(uSpecularEnvSampler, reflection)).rgb;
#endif

  vec3 diffuse = diffuseLight * pbrInputs.diffuseColor;
  vec3 specular = specularLight * (pbrInputs.specularColor * brdf.x + brdf.y);

  return diffuse + specular;
}
// Calculation of the lighting contribution from an optional Image Based Light source.
// Precomputed Environment Maps are required uniform inputs and are computed as outlined in [1].
// See our README.md on Environment Maps [3] for additional discussion.
//----------------------------------------------------------------------------------------------------------------------
vec3 GetIBLContribution(vec3 diffuseColor, vec3 specularColor, float perceptualRoughness, float NdotV, vec3 n, vec3 reflection)
{
  // retrieve a scale and bias to F0. See [1], Figure 3
  vec3 brdf = SRGBtoLINEAR(texture2D(ubrdfLUT, vec2(NdotV, 1.0 - perceptualRoughness))).rgb;
  vec3 diffuseLight = SRGBtoLINEAR(textureCube(uDiffuseEnvSampler, n)).rgb;

#ifdef USE_TEX_LOD
  const float mipCount = 9.0;// resolution of 512x512
  float lod = (perceptualRoughness * mipCount);
  vec3 specularLight = SRGBtoLINEAR(textureCubeLod(uSpecularEnvSampler, reflection, lod)).rgb;
#else
  vec3 specularLight = SRGBtoLINEAR(textureCube(uSpecularEnvSampler, reflection)).rgb;
#endif

  vec3 diffuse = diffuseLight * diffuseColor;
  vec3 specular = specularLight * (specularColor * brdf.x + brdf.y);

  return diffuse + specular;
}

#endif
