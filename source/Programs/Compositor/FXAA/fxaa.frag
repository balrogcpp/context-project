// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"
#include "math.glsl"


in mediump vec2 vUV0;
uniform sampler2D ColorMap;
uniform vec2 TexelSize0;
uniform float Scale;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    #define FXAA_REDUCE_MIN 0.0078125 // 1/128
    #define FXAA_REDUCE_MUL 0.125 // 1.0/8.0
    #define FXAA_SPAN_MAX 8.0

    mediump vec3 rgbNW = texture2D(ColorMap, vUV0 + vec2(-1.0, -1.0) * TexelSize0).xyz;
    mediump vec3 rgbNE = texture2D(ColorMap, vUV0 + vec2(1.0, -1.0) * TexelSize0).xyz;
    mediump vec3 rgbSW = texture2D(ColorMap, vUV0 + vec2(-1.0, 1.0) * TexelSize0).xyz;
    mediump vec3 rgbSE = texture2D(ColorMap, vUV0 + vec2(1.0, 1.0) * TexelSize0).xyz;
    mediump vec3 rgbM  = texture2D(ColorMap,  vUV0).xyz;
    const mediump vec3 luma = vec3(0.299, 0.587, 0.114);
    mediump float lumaNW = dot(rgbNW, luma);
    mediump float lumaNE = dot(rgbNE, luma);
    mediump float lumaSW = dot(rgbSW, luma);
    mediump float lumaSE = dot(rgbSE, luma);
    mediump float lumaM  = dot(rgbM,  luma);
    mediump float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    mediump float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
    mediump vec2 dir = vec2(0.0, 0.0);
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    mediump float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    mediump float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(vec2(FXAA_SPAN_MAX,  FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) * TexelSize0;
    mediump vec3 rgbA = 0.5 * (texture2D(ColorMap, vUV0 + dir * (1.0/3.0 - 0.5)).xyz + texture2D(ColorMap, vUV0 + dir * (2.0/3.0 - 0.5)).xyz);
    mediump vec3 rgbB = rgbA * 0.5 + 0.25 * (texture2D(ColorMap,  vUV0 + dir * -0.5).xyz + texture2D(ColorMap,  vUV0 + dir * 0.5).xyz);
    mediump float lumaB = dot(rgbB, luma);
    if((lumaB < lumaMin) || (lumaB > lumaMax)) {
        FragColor = vec4(SafeHDR(rgbA), 1.0);
    } else {
        FragColor = vec4(SafeHDR(rgbB), 1.0);
    }
}
