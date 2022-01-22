// This source file is part of "glue project". Created by Andrey Vasiliev

#ifndef GL_ES
#version 120
#define VERSION 120
#else
#version 100
#define VERSION 100
#endif

#include "header.frag"

in vec2 oUv0;
uniform sampler2D sSceneDepthSampler;
uniform sampler2D sRotSampler4x4;
uniform vec4 cViewportSize; // auto param width/height/inv. width/inv. height
uniform float cFov; // vertical field of view in radians
uniform float nearClipDistance;
uniform float farClipDistance;
uniform float cSampleInScreenspace; // whether to sample in screen or world space
uniform float cSampleLengthScreenSpace; // The sample length in screen space [0, 1]
uniform float cSampleLengthWorldSpace; // the sample length in world space in units
uniform float cOffsetScale; // [0, 1] The distance of the first sample. samples are the
// placed in [cOffsetScale * cSampleLengthScreenSpace, cSampleLengthScreenSpace]
uniform float cDefaultAccessibility; // the default value used in the lerp() expression for invalid samples [0, 1]
uniform float cEdgeHighlight; // multiplier for edge highlighting in [1, 2] 1 is full highlighting 2 is off
uniform float shadow_colour;
uniform vec4 fog_params;

//----------------------------------------------------------------------------------------------------------------------
void main()
{
//  const float nSampleNum = 32.0; // number of samples
  const float nSampleNum = 8.0; // number of samples

  // get the depth of the current pixel and convert into world space unit [0, inf]
  float clampedDepth = texture2D(sSceneDepthSampler, oUv0).r;
  float fragmentWorldDepth = clampedDepth * farClipDistance - nearClipDistance;

  if (fragmentWorldDepth <= 0.0) {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    return;
  }


  float accessibility = 0.0;

  // get rotation vector, rotation is tiled every 4 screen pixels
  vec2 rotationTC = oUv0 * cViewportSize.xy / 4.0;
  vec3 rotationVector = 2.0 * texture2D(sRotSampler4x4, rotationTC).xyz - 1.0;// [-1, 1]x[-1. 1]x[-1. 1]

  float rUV = 0.0;// radius of influence in screen space
  float r = 0.0;// radius of influence in world space
  if (cSampleInScreenspace == 1.0)
  {
    rUV = cSampleLengthScreenSpace;
    r = tan(rUV * cFov) * fragmentWorldDepth;
  }
  else
  {
    rUV = atan(cSampleLengthWorldSpace / fragmentWorldDepth) / cFov;// the radius of influence projected into screen space
    r = cSampleLengthWorldSpace;
  }

  float sampleLength = cOffsetScale;// the offset for the first sample
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

      vec2 sampleTC = oUv0 + rotatedOffset.xy * rUV;

      // read scene depth at sampling point and convert into world space units (m or whatever)
      float sampleWorldDepth = texture2D(sSceneDepthSampler, sampleTC).r * farClipDistance;

      // check if depths of both pixels are close enough and sampling point should affect our center pixel
      float fRangeIsInvalid = clamp((fragmentWorldDepth - sampleWorldDepth) / r, 0.0, 1.0);

      // accumulate accessibility, use default value of 0.5 if right computations are not possible

      accessibility += mix(float(sampleWorldDepth > (fragmentWorldDepth + rotatedOffset.z * r)), cDefaultAccessibility, fRangeIsInvalid);
    }
  }

  // get average value
  accessibility /= nSampleNum;

  // normalize, remove edge highlighting
  accessibility *= cEdgeHighlight;

  accessibility = clamp(accessibility + shadow_colour, 0.0, 1.0);

  float exponent = fragmentWorldDepth * fog_params.x;
  float fog_value = clamp(1.0 / exp(exponent), 0.0, 1.0);
  accessibility = mix(1.0, accessibility, fog_value);
//  float fog_value = 1.0 - clamp(1.0 / exp(exponent), 0.0, 1.0);
//  accessibility = mix(accessibility, 1.0, fog_value);

  // amplify and saturate if necessary
  FragColor = vec4(vec3(accessibility), 1.0);
}
