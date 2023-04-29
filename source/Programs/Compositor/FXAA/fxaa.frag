// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"
#include "srgb.glsl"


in mediump vec2 vUV0;
uniform sampler2D ColorMap;
uniform vec2 TexelSize0;


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Fxaa(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    #define FXAA_REDUCE_MIN (1.0/16.0)
    #define FXAA_REDUCE_MUL (1.0/8.0)
    #define FXAA_SPAN_MAX 8.0

    mediump vec3 rgbNW = texture2D(tex, uv + vec2(-1.0, -1.0) * tsize).xyz;
    mediump vec3 rgbNE = texture2D(tex, uv + vec2( 1.0, -1.0) * tsize).xyz;
    mediump vec3 rgbSW = texture2D(tex, uv + vec2(-1.0,  1.0) * tsize).xyz;
    mediump vec3 rgbSE = texture2D(tex, uv + vec2( 1.0,  1.0) * tsize).xyz;
    mediump vec3 rgbM  = texture2D(tex, uv                           ).xyz;

//    const mediump vec3 luma = vec3(0.299, 0.587, 0.114);
    const mediump vec3 luma = vec3(0.2126, 0.7152, 0.0722);
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
    dir = min(vec2(FXAA_SPAN_MAX,  FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) * tsize;

    mediump vec3 rgbA = 0.5 * (texture2D(tex, uv + dir * (1.0/3.0 - 0.5)).xyz + texture2D(tex, uv + dir * (2.0/3.0 - 0.5)).xyz);
    mediump vec3 rgbB = rgbA * 0.5 + 0.25 * (texture2D(tex,  uv + dir * -0.5).xyz + texture2D(tex,  uv + dir * 0.5).xyz);
    mediump float lumaB = dot(rgbB, luma);

    if((lumaB < lumaMin) || (lumaB > lumaMax)) {
        return rgbA;
    } else {
        return rgbB;
    }
}


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    FragColor = vec4(SafeHDR(Fxaa(ColorMap, vUV0, TexelSize0)), 1.0);
}
