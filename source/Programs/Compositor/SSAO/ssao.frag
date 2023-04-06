// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.frag"


in vec2 vUV0;
uniform sampler2D DepthMap;
uniform sampler2D NormalMap;
uniform mat4 ProjMatrix;
uniform mat4 InvProjMatrix;
uniform mat4 InvViewMatrix;
uniform float FarClipDistance;


//----------------------------------------------------------------------------------------------------------------------
vec3 hash(vec3 a)
{
  a = fract(a * vec3(0.8));
  a += dot(a, a.yxz + 19.19);
  return fract((a.xxy + a.yxx) * a. zyx);
}


//----------------------------------------------------------------------------------------------------------------------
vec3 getPositionFromDepth(const vec2 uv, const float depth)
{
  vec4 viewSpacePosition = InvProjMatrix * vec4(uv, depth, 1.0);

  // Perspective division (?)
  //viewSpacePosition /= viewSpacePosition.w;

  return viewSpacePosition.xyz;
}


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  #define MAX_RAND_SAMPLES 14

  vec3 RAND_SAMPLES[MAX_RAND_SAMPLES];
  RAND_SAMPLES[0] = vec3(1.0, 0.0, 0.0);
  RAND_SAMPLES[1] = vec3(-1.0, 0.0, 0.0);
  RAND_SAMPLES[2] = vec3(0.0, 1.0, 0.0);
  RAND_SAMPLES[3] = vec3(0.0, -1.0, 0.0);
  RAND_SAMPLES[4] = vec3(0.0, 0.0, 1.0);
  RAND_SAMPLES[5] = vec3(0.0, 0.0, -1.0);
  RAND_SAMPLES[6] = normalize(vec3(1.0, 1.0, 1.0));
  RAND_SAMPLES[7] = normalize(vec3(-1.0, 1.0, 1.0));
  RAND_SAMPLES[8] = normalize(vec3(1.0, -1.0, 1.0));
  RAND_SAMPLES[9] = normalize(vec3(1.0, 1.0, -1.0));
  RAND_SAMPLES[10] = normalize(vec3(-1.0, -1.0, 1.0));
  RAND_SAMPLES[11] = normalize(vec3(-1.0, 1.0, -1.0));
  RAND_SAMPLES[12] = normalize(vec3(1.0, -1.0, -1.0));
  RAND_SAMPLES[13] = normalize(vec3(-1.0, -1.0, -1.0));


  // constant expression != const int :(
  #define NUM_BASE_SAMPLES MAX_RAND_SAMPLES

  // random normal lookup from a texture and expand to [-1..1]
  float depth = texture2D(DepthMap, vUV0).x;

  // IN.ray will be distorted slightly due to interpolation
  // it should be normalized here
  vec3 viewPos = getPositionFromDepth(vUV0, depth);
  vec3 worldPos = vec3(InvViewMatrix * vec4(viewPos, 1.0));
  vec3 randN = hash(worldPos);

  // By computing Z manually, we lose some accuracy under extreme angles
  // considering this is just for bias, this loss is acceptable
  vec3 viewNorm = texture2D(NormalMap, vUV0).xyz;

  // Accumulated occlusion factor
  float occ = 0.0;
  for (int i = 0; i < NUM_BASE_SAMPLES; ++i)
  {
      // Reflected direction to move in for the sphere
      // (based on random samples and a random texture sample)
      // bias the random direction away from the normal
      // this tends to minimize self occlusion
      vec3 randomDir = reflect(RAND_SAMPLES[i], randN) + viewNorm;

      // Move new view-space position back into texture space
      //#define RADIUS 0.2125
      #define RADIUS 0.0525

      vec4 nuv = ProjMatrix * vec4(viewPos + randomDir * RADIUS, 1.0);
      nuv /= nuv.w;

      // Compute occlusion based on the (scaled) Z difference
      float zd = clamp(FarClipDistance * (depth - texture2D(DepthMap, nuv.xy).x), 0.0, 1.0);
      // This is a sample occlusion function, you can always play with
      // other ones, like 1.0 / (1.0 + zd * zd) and stuff
      occ += clamp(pow(1.0 - zd, 11.0) + zd, 0.0, 1.0);
  }

  // normalise
  occ /= float(NUM_BASE_SAMPLES);

  // amplify and saturate if necessary
  FragColor.r = occ;
}
