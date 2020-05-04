/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

Copyright (c) 2000-2014 Torus Knot Software Ltd
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
//-----------------------------------------------------------------------------
// Program Name: SGXLib_IntegratedPSSM
// Program Desc: Integrated PSSM functions.
// Program Type: Vertex/Pixel shader
// Language: GLSL
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void SGX_ApplyShadowFactor_Diffuse(in vec4 ambient, 
					  in vec4 lightSum, 
					  in float fShadowFactor, 
					  out vec4 oLight)
{
	oLight.rgb = ambient.rgb + (lightSum.rgb - ambient.rgb) * fShadowFactor;
	oLight.a   = lightSum.a;
}
	
//-----------------------------------------------------------------------------
#define NUM_SHADOW_SAMPLES_1D 4.0
#define SHADOW_FILTER_SCALE 1
#define SHADOW_SAMPLES NUM_SHADOW_SAMPLES_1D * NUM_SHADOW_SAMPLES_1D

vec4 offsetSample(vec4 uv, vec2 offset, float invMapSize)
{
	return vec4(uv.xy + offset * invMapSize * uv.w, uv.z, uv.w);
}

void SGX_ShadowPCF4(in sampler2D shadowMap, in vec4 uv, in vec2 invShadowMapSize, out float shadow)
{
	// 4-sample PCF
	shadow = 0.0;
	float offset = (NUM_SHADOW_SAMPLES_1D/2 - 0.5) * SHADOW_FILTER_SCALE;
	uv /= uv.w;
	uv.z = uv.z * 0.5 + 0.5; // convert -1..1 to 0..1
	float attenuation = 1.0;

	const int iterations = 16;

	const vec2 poissonDisk16[16] = vec2[](
	vec2( -0.94201624, -0.39906216 ),
	vec2( 0.94558609, -0.76890725 ),
	vec2( -0.094184101, -0.92938870 ),
	vec2( 0.34495938, 0.29387760 ),
	vec2( -0.91588581, 0.45771432 ),
	vec2( -0.81544232, -0.87912464 ),
	vec2( -0.38277543, 0.27676845 ),
	vec2( 0.97484398, 0.75648379 ),
	vec2( 0.44323325, -0.97511554 ),
	vec2( 0.53742981, -0.47373420 ),
	vec2( -0.26496911, -0.41893023 ),
	vec2( 0.79197514, 0.19090188 ),
	vec2( -0.24188840, 0.99706507 ),
	vec2( -0.81409955, 0.91437590 ),
	vec2( 0.19984126, 0.78641367 ),
	vec2( 0.14383161, -0.14100790 )
	);

	for (float y = -offset; y <= offset; y += SHADOW_FILTER_SCALE)
	for (float x = -offset; x <= offset; x += SHADOW_FILTER_SCALE)
	for (int i = 0; i < iterations; i++)
	{
		vec2 uv2 = uv.xy + vec2(x, y) * invShadowMapSize.xy + poissonDisk16[i]/700.0;
		float depth = texture2D(shadowMap, uv2).r + 1.0;
		shadow += float(depth > uv.z);
	}

	shadow *= attenuation / (SHADOW_SAMPLES * iterations);
}

void SGX_ShadowPCF4_(in sampler2D shadowMap, in vec4 shadowMapPos, in vec2 offset, out float c)
{
	shadowMapPos = shadowMapPos / shadowMapPos.w;
#ifndef OGRE_REVERSED_Z
	shadowMapPos.z = shadowMapPos.z * 0.5 + 0.5; // convert -1..1 to 0..1
#endif
	vec2 uv = shadowMapPos.xy;
	vec3 o = vec3(offset, -offset.x) * 0.3;

	// Note: We using 2x2 PCF. Good enough and is a lot faster.
	c =	 (shadowMapPos.z <= texture2D(shadowMap, uv.xy - o.xy).r + 1.0) ? 1.0 : 0.0; // top left
	c += (shadowMapPos.z <= texture2D(shadowMap, uv.xy + o.xy).r + 1.0) ? 1.0 : 0.0; // bottom right
	c += (shadowMapPos.z <= texture2D(shadowMap, uv.xy + o.zy).r + 1.0) ? 1.0 : 0.0; // bottom left
	c += (shadowMapPos.z <= texture2D(shadowMap, uv.xy - o.zy).r + 1.0) ? 1.0 : 0.0; // top right

	c /= 4.0;
#ifdef OGRE_REVERSED_Z
    c = 1.0 - c;
#endif
}

void SGX_ShadowPCF4(in sampler2DShadow shadowMap, in vec4 shadowMapPos, out float c)
{
#ifndef GL_ES
#ifndef OGRE_REVERSED_Z
    shadowMapPos.z = shadowMapPos.z * 0.5 + 0.5 * shadowMapPos.w;// convert -1..1 to 0..1
#endif
    c = vec4(shadow2DProj(shadowMap, shadowMapPos)).r + 1.0;// avoid scalar swizzle with textureProj
#endif
}

//-----------------------------------------------------------------------------
void SGX_ComputeShadowFactor_PSSM3(in float fDepth,
							in vec4 vSplitPoints,	
							in vec4 lightPosition0,
							in vec4 lightPosition1,
							in vec4 lightPosition2,
							in sampler2D shadowMap0,
							in sampler2D shadowMap1,
							in sampler2D shadowMap2,
							in vec4 invShadowMapSize0,
							in vec4 invShadowMapSize1,
							in vec4 invShadowMapSize2,																			
							out float oShadowFactor)
{
	if (fDepth  <= vSplitPoints.x)
	{
		SGX_ShadowPCF4(shadowMap0, lightPosition0, invShadowMapSize0.xy, oShadowFactor);
	}
	else if (fDepth <= vSplitPoints.y)
	{
		SGX_ShadowPCF4(shadowMap1, lightPosition1, invShadowMapSize1.xy, oShadowFactor);
	}
	else
	{
		SGX_ShadowPCF4(shadowMap2, lightPosition2, invShadowMapSize2.xy, oShadowFactor);
	}
}

void SGX_ComputeShadowFactor_PSSM3(in float fDepth,
							in vec4 vSplitPoints,
							in vec4 lightPosition0,
							in vec4 lightPosition1,
							in vec4 lightPosition2,
							in sampler2DShadow shadowMap0,
							in sampler2DShadow shadowMap1,
							in sampler2DShadow shadowMap2,
							out float oShadowFactor)
{
	if (fDepth  <= vSplitPoints.x)
	{
        SGX_ShadowPCF4(shadowMap0, lightPosition0, oShadowFactor);
	}
	else if (fDepth <= vSplitPoints.y)
	{
        SGX_ShadowPCF4(shadowMap1, lightPosition1, oShadowFactor);
	}
	else
	{
        SGX_ShadowPCF4(shadowMap2, lightPosition2, oShadowFactor);
	}
}