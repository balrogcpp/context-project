// This source file is part of "glue project". Created by Andrey Vasiliev

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
