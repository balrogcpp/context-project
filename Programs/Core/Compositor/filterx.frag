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
uniform sampler2D uSampler;
uniform vec2 texelSize;

vec3 Iteration(const float offset, const float weight) {
  return texture2D(uSampler, (oUv0 + vec2(offset, 0.0) * texelSize.x)).rgb * weight + texture2D(uSampler, (oUv0 - vec2(offset, 0.0) * texelSize.x)).rgb * weight;
}

void main()
{
//  const int radius = 3;
//  const float offset[radius] = float[]( 0.0, 1.3846153846, 3.2307692308 );
//  const float weight[radius] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );
//
//  const float offset0 = 0.0;
//  const float offset1 = 1.3846153846;
//  const float offset2 = 3.2307692308;
//
//  const float weight0 = 0.2270270270;
//  const float weight1 = 0.3162162162;
//  const float weight2 = 0.0702702703;
//
//  vec3 final_color = texture2D(uSampler, oUv0, lod).rgb * weight0;
//
//  final_color += Iteration(offset1, weight1);
//  final_color += Iteration(offset2, weight2);
//
//  const int radius = 5;
//  const float offset[5] = float[](0.0, 1.0, 2.0, 3.0, 4.0);
//  const float weight[5] = float[](0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

  const float offset0 = 0.0;
  const float offset1 = 1.0;
  const float offset2 = 2.0;
  const float offset3 = 3.0;
  const float offset4 = 4.0;

  const float weight0 = 0.2270270270;
  const float weight1 = 0.1945945946;
  const float weight2 = 0.1216216216;
  const float weight3 = 0.0540540541;
  const float weight4 = 0.0162162162;

  vec3 final_color = texture2D(uSampler, oUv0).rgb * weight0;

  final_color += Iteration(offset1, weight1);
  final_color += Iteration(offset2, weight2);
  final_color += Iteration(offset3, weight3);
  final_color += Iteration(offset4, weight4);

  FragColor = vec4(final_color, 1.0);
}
