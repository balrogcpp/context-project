// This source file is part of Glue Engine. Created by Andrey Vasiliev

#ifndef GL_ES
#version 120
#define VERSION 120
#else
#version 100
#define VERSION 100
#endif

#include "header.frag"

in vec2 vUV0;
uniform sampler2D uSceneDepthSampler;
uniform vec2 TexelSize;
uniform sampler2D sRotSampler4x4;
uniform vec4 ViewportSize; // auto param width/height/inv. width/inv. height
uniform float FOV; // vertical field of view in radians
uniform float NearClipDistance;
uniform float FarClipDistance;
uniform float uSampleInScreenspace; // whether to sample in screen or world space
uniform float uSampleLengthScreenSpace; // The sample length in screen space [0, 1]
uniform float uSampleLengthWorldSpace; // the sample length in world space in units
uniform float uOffsetScale; // [0, 1] The distance of the first sample. samples are the
// placed in [uOffsetScale * uSampleLengthScreenSpace, uSampleLengthScreenSpace]
uniform float uDefaultAccessibility; // the default value used in the lerp() expression for invalid samples [0, 1]
uniform float uEdgeHighlight; // multiplier for edge highlighting in [1, 2] 1 is full highlighting 2 is off
uniform float uEnable;

//----------------------------------------------------------------------------------------------------------------------
void main()
{
  if (uEnable <= 0.0) discard;

//   const float nSampleNum = 32.0; // number of samples
  const float nSampleNum = 16.0; // number of samples

  // get the depth of the current pixel and convert into world space unit [0, inf]
  float clampedDepth = texture2D(uSceneDepthSampler, vUV0).r;
  float fragmentWorldDepth = clampedDepth * FarClipDistance - NearClipDistance;

  if (fragmentWorldDepth <= 0.0)
  {
    FragColor.r = 1.0;
    return;
  }

  float accessibility = 0.0;

  // get rotation vector, rotation is tiled every 4 screen pixels
  vec2 rotationTC = vUV0 * ViewportSize.xy / 4.0;
  vec3 rotationVector = 2.0 * texture2D(sRotSampler4x4, rotationTC).xyz - 1.0;// [-1, 1]x[-1. 1]x[-1. 1]

  float rUV = 0.0;// radius of influence in screen space
  float r = 0.0;// radius of influence in world space
  if (uSampleInScreenspace == 1.0)
  {
    rUV = uSampleLengthScreenSpace;
    r = tan(rUV * FOV) * fragmentWorldDepth;
  }
  else
  {
    rUV = atan(uSampleLengthWorldSpace / fragmentWorldDepth) / FOV;// the radius of influence projected into screen space
    r = uSampleLengthWorldSpace;
  }

  float sampleLength = uOffsetScale;// the offset for the first sample
  float sampleLengthStep = pow((rUV / sampleLength), 1.0/nSampleNum);

  // sample the sphere and accumulate accessibility
  for (int i = 0; i < (int(nSampleNum)/8); i++)
  {
    for (int x = -1; x <= 1; x += 2)
    for (int y = -1; y <= 1; y += 2)
    for (int z = -1; z <= 1; z += 2)
    {
      //generate offset vector
      vec3 offset = normalize(vec3(x, y, z)) * sampleLength;

      // update sample length
      sampleLength *= sampleLengthStep;

      // reflect offset vector by random rotation sample (i.e. rotating it)
      vec3 rotatedOffset = reflect(offset, rotationVector);

      vec2 sampleTC = vUV0 + rotatedOffset.xy * rUV;

      // read scene depth at sampling point and convert into world space units (m or whatever)
      float sampleWorldDepth = texture2D(uSceneDepthSampler, sampleTC).r * FarClipDistance;

      // check if depths of both pixels are close enough and sampling point should affect our center pixel
      float fRangeIsInvalid = clamp((fragmentWorldDepth - sampleWorldDepth) / r, 0.0, 1.0);

      // accumulate accessibility, use default value of 0.5 if right computations are not possible

      accessibility += mix(float(sampleWorldDepth > (fragmentWorldDepth + rotatedOffset.z * r)), uDefaultAccessibility, fRangeIsInvalid);
    }
  }

  // get average value
  accessibility /= nSampleNum;

  // normalize, remove edge highlighting
  accessibility *= uEdgeHighlight;

  // amplify and saturate if necessary
  FragColor.r = accessibility;
}
