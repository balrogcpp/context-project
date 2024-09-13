//
// KinoFog - Deferred fog effect
//
// Copyright (C) 2015 Keijiro Takahashi
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include "header.glsl"
#include "math.glsl"
#include "fog.glsl"

uniform sampler2D RT;
uniform sampler2D DepthTex;
uniform samplerCube SkyBoxTex;
uniform highp vec3 CameraPosition;
uniform vec3 LightDir0;
uniform vec4 FogColour;
uniform vec4 FogParams;
uniform mat4 InvViewMatrix;
uniform float ClipDistance;

in highp vec2 vUV0;
in highp vec3 vRay;
void main()
{
    vec3 color = texture2D(RT, vUV0).rgb;
    float clampedPixelDepth = texture2D(DepthTex, vUV0).x;

    if (clampedPixelDepth > 0.9) {
        FragColor.rgb = color;
        return;
    }

    float pixelDepth = clampedPixelDepth * ClipDistance;
    vec3 viewPos = vRay * clampedPixelDepth;
    vec4 wPosition = mulMat4x4Half3(InvViewMatrix, viewPos.xyz);
    wPosition.xy /= wPosition.w;
    vec3 v = normalize(wPosition.xyz - CameraPosition);
    vec3 fogColor = textureCube(SkyBoxTex, vec3(-v.x, v.y, v.z)).rgb;
    vec3 fog = ApplyFog(color, FogParams.x, fogColor, max(pixelDepth - 3.0, 0.0), v, LightDir0.xyz, CameraPosition);
    FragColor.rgb = fog;
}
