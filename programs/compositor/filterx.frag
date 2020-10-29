//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
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

#ifndef GL_ES
#define VERSION 120
#version VERSION
#else
#define VERSION 100
#version VERSION
#endif
#include "header.frag"
in vec2 oUv0;
uniform sampler2D uSampler;
uniform vec2 texelSize;

vec4 Iteration(float offset, float weight) {
  return texture2D(uSampler, (oUv0 + vec2(offset, 0.0) * texelSize.x)) * weight +
      texture2D(uSampler, (oUv0 - vec2(offset, 0.0) * texelSize.x)) * weight;
}

void main()
{
  //  const int radius = 3;
  //  const float offset[radius] = float[]( 0.0, 1.3846153846, 3.2307692308 );
  //  const float weight[radius] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );

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

  vec4 final_color = texture2D(uSampler, oUv0) * weight0;

  final_color += Iteration(offset1, weight1);
  final_color += Iteration(offset2, weight2);
  final_color += Iteration(offset3, weight3);
  final_color += Iteration(offset4, weight4);

  gl_FragColor = final_color;
}