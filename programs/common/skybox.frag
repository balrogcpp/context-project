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
#define VERSION 120
#version VERSION
#else
#define VERSION 100
#version VERSION
#endif
#include "header.frag"
#include "srgb.glsl"

uniform samplerCube cubemap;
uniform vec3 uFogColour;
uniform vec4 uFogParams;

in float vDepth;
in vec3 TexCoords; // direction vector representing a 3D texture coordinate

//----------------------------------------------------------------------------------------------------------------------
vec3 ApplyFog(vec3 color) {
    float exponent = vDepth * uFogParams.x;
    float fog_value = 1.0 - clamp(1.0 / exp(exponent), 0.0, 1.0);
    return mix(color, uFogColour, fog_value);
}

void main()
{
    vec3 color = SRGBtoLINEAR(textureCube(cubemap, TexCoords).rgb);

    color = ApplyFog(color);

    gl_FragColor = vec4(color, 1.0);
}
