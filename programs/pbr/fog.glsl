//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
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

vec3 ApplyFog(vec3 color, vec4 fog_params, vec3 fog_color, float depth) {
    float exponent = depth * fog_params.x;
    float fog_value = 1.0 - clamp(1.0 / exp(exponent), 0.0, 1.0);
    return mix(color, fog_color, fog_value);
}

#else

float Exp(float x) {
    return 1.0 + x + x * x / 2.0;
}

vec3 ApplyFog(vec3 color, vec4 fog_params, vec3 fog_color, float depth) {
    float exponent = depth * fog_params.x;
    float fog_value = 1.0 - clamp(1.0 / Exp(exponent), 0.0, 1.0);
    return mix(color, fog_color, fog_value);
}

#endif
