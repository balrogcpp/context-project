// created by Andrey Vasiliev

#ifndef __VERSION__
#if defined(OGRE_GLSL)
#version 330 core
#define __VERSION__ 330
#elif defined(OGRE_GLSLES)
#version 300 es
#define __VERSION__ 300
#endif
#endif

#ifndef FXAA_REDUCE_MIN
#define FXAA_REDUCE_MIN   (1.0/ 128.0)
#endif
#ifndef FXAA_REDUCE_MUL
#define FXAA_REDUCE_MUL   (1.0 / 8.0)
#endif
#ifndef FXAA_SPAN_MAX
#define FXAA_SPAN_MAX     8.0
#endif

#include "header.glsl"

uniform sampler2D RT;
uniform mediump vec2 TexelSize0;

// https://github.com/mattdesl/glsl-fxaa/blob/master/fxaa.glsl
mediump vec3 FastFxaa(sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 rgbNW = texture2D(tex, uv + vec2(-1.0, -1.0) * tsize).xyz;
    mediump vec3 rgbNE = texture2D(tex, uv + vec2( 1.0, -1.0) * tsize).xyz;
    mediump vec3 rgbSW = texture2D(tex, uv + vec2(-1.0,  1.0) * tsize).xyz;
    mediump vec3 rgbSE = texture2D(tex, uv + vec2( 1.0,  1.0) * tsize).xyz;
    mediump vec3 rgbM  = texture2D(tex, uv                           ).xyz;

    const mediump vec3 luma = vec3(0.2126, 0.7152, 0.0722);
    
    mediump float lumaNW = dot(rgbNW, luma);
    mediump float lumaNE = dot(rgbNE, luma);
    mediump float lumaSW = dot(rgbSW, luma);
    mediump float lumaSE = dot(rgbSE, luma);
    mediump float lumaM  = dot(rgbM,  luma);

    mediump float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    mediump float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    mediump vec2 dir;
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


//https://github.com/libretro/glsl-shaders/blob/master/anti-aliasing/shaders/fxaa.glsl

/*
FXAA_PRESET - Choose compile-in knob preset 0-5.
------------------------------------------------------------------------------
FXAA_EDGE_THRESHOLD - The minimum amount of local contrast required 
                      to apply algorithm.
                      1.0/3.0  - too little
                      1.0/4.0  - good start
                      1.0/8.0  - applies to more edges
                      1.0/16.0 - overkill
------------------------------------------------------------------------------
FXAA_EDGE_THRESHOLD_MIN - Trims the algorithm from processing darks.
                          Perf optimization.
                          1.0/32.0 - visible limit (smaller isn't visible)
                          1.0/16.0 - good compromise
                          1.0/12.0 - upper limit (seeing artifacts)
------------------------------------------------------------------------------
FXAA_SEARCH_STEPS - Maximum number of search steps for end of span.
------------------------------------------------------------------------------
FXAA_SEARCH_THRESHOLD - Controls when to stop searching.
                        1.0/4.0 - seems to be the best quality wise
------------------------------------------------------------------------------
FXAA_SUBPIX_TRIM - Controls sub-pixel aliasing removal.
                   1.0/2.0 - low removal
                   1.0/3.0 - medium removal
                   1.0/4.0 - default removal
                   1.0/8.0 - high removal
                   0.0 - complete removal
------------------------------------------------------------------------------
FXAA_SUBPIX_CAP - Insures fine detail is not completely removed.
                  This is important for the transition of sub-pixel detail,
                  like fences and wires.
                  3.0/4.0 - default (medium amount of filtering)
                  7.0/8.0 - high amount of filtering
                  1.0 - no capping of sub-pixel aliasing removal
*/

#ifndef FXAA_PRESET
    #define FXAA_PRESET 4
#endif
#if (FXAA_PRESET == 3)
    #define FXAA_EDGE_THRESHOLD      (1.0/8.0)
    #define FXAA_EDGE_THRESHOLD_MIN  (1.0/16.0)
    #define FXAA_SEARCH_STEPS        16
    #define FXAA_SEARCH_THRESHOLD    (1.0/4.0)
    #define FXAA_SUBPIX_CAP          (3.0/4.0)
    #define FXAA_SUBPIX_TRIM         (1.0/4.0)
#endif
#if (FXAA_PRESET == 4)
    #define FXAA_EDGE_THRESHOLD      (1.0/8.0)
    #define FXAA_EDGE_THRESHOLD_MIN  (1.0/24.0)
    #define FXAA_SEARCH_STEPS        24
    #define FXAA_SEARCH_THRESHOLD    (1.0/4.0)
    #define FXAA_SUBPIX_CAP          (3.0/4.0)
    #define FXAA_SUBPIX_TRIM         (1.0/4.0)
#endif
#if (FXAA_PRESET == 5)
    #define FXAA_EDGE_THRESHOLD      (1.0/8.0)
    #define FXAA_EDGE_THRESHOLD_MIN  (1.0/24.0)
    #define FXAA_SEARCH_STEPS        32
    #define FXAA_SEARCH_THRESHOLD    (1.0/4.0)
    #define FXAA_SUBPIX_CAP          (3.0/4.0)
    #define FXAA_SUBPIX_TRIM         (1.0/4.0)
#endif

#define FXAA_SUBPIX_TRIM_SCALE (1.0/(1.0 - FXAA_SUBPIX_TRIM))

// Return the luma, the estimation of luminance from rgb inputs.
// This approximates luma using one FMA instruction,
// skipping normalization and tossing out blue.
// FxaaLuma() will range 0.0 to 2.963210702.
mediump float FxaaLuma(const mediump vec3 rgb) {
    //return rgb.y * (0.587/0.299) + rgb.x;
    return dot(rgb, vec3(0.2126, 0.7152, 0.0722));
}

mediump vec3 FxaaLerp3(const mediump vec3 a, const mediump vec3 b, const mediump float amountOfA) {
    return (vec3(-amountOfA, -amountOfA, -amountOfA) * b) + ((a * vec3(amountOfA, amountOfA, amountOfA)) + b);
}

vec4 FxaaTexOff(sampler2D tex, const mediump vec2 pos, const mediump vec2 off, const mediump vec2 rcpFrame) {
    mediump float x = pos.x + off.x * rcpFrame.x;
    mediump float y = pos.y + off.y * rcpFrame.y;
    return texture2D(tex, vec2(x, y));
}

// pos is the output of FxaaVertexShader interpolated across screen.
// xy -> actual texture position {0.0 to 1.0}
// rcpFrame should be a uniform equal to  {1.0/frameWidth, 1.0/frameHeight}
mediump vec3 FxaaPixelShader(const mediump vec2 pos, sampler2D tex, const mediump vec2 rcpFrame)
{
    mediump vec3 rgbN = FxaaTexOff(tex, pos.xy, vec2( 0.0, -1.0), rcpFrame).xyz;
    mediump vec3 rgbW = FxaaTexOff(tex, pos.xy, vec2(-1.0,  0.0), rcpFrame).xyz;
    mediump vec3 rgbM = FxaaTexOff(tex, pos.xy, vec2( 0.0,  0.0), rcpFrame).xyz;
    mediump vec3 rgbE = FxaaTexOff(tex, pos.xy, vec2( 1.0,  0.0), rcpFrame).xyz;
    mediump vec3 rgbS = FxaaTexOff(tex, pos.xy, vec2( 0.0,  1.0), rcpFrame).xyz;
    
    mediump float lumaN = FxaaLuma(rgbN);
    mediump float lumaW = FxaaLuma(rgbW);
    mediump float lumaM = FxaaLuma(rgbM);
    mediump float lumaE = FxaaLuma(rgbE);
    mediump float lumaS = FxaaLuma(rgbS);
    mediump float rangeMin = min(lumaM, min(min(lumaN, lumaW), min(lumaS, lumaE)));
    mediump float rangeMax = max(lumaM, max(max(lumaN, lumaW), max(lumaS, lumaE)));
    
    mediump float range = rangeMax - rangeMin;
    if(range < max(FXAA_EDGE_THRESHOLD_MIN, rangeMax * FXAA_EDGE_THRESHOLD))
    {
        return rgbM;
    }
    
    mediump vec3 rgbL = rgbN + rgbW + rgbM + rgbE + rgbS;
    
    mediump float lumaL = (lumaN + lumaW + lumaE + lumaS) * 0.25;
    mediump float rangeL = abs(lumaL - lumaM);
    mediump float blendL = max(0.0, (rangeL / range) - FXAA_SUBPIX_TRIM) * FXAA_SUBPIX_TRIM_SCALE; 
    blendL = min(FXAA_SUBPIX_CAP, blendL);
    
    mediump vec3 rgbNW = FxaaTexOff(tex, pos.xy, vec2(-1.0, -1.0), rcpFrame).xyz;
    mediump vec3 rgbNE = FxaaTexOff(tex, pos.xy, vec2( 1.0, -1.0), rcpFrame).xyz;
    mediump vec3 rgbSW = FxaaTexOff(tex, pos.xy, vec2(-1.0,  1.0), rcpFrame).xyz;
    mediump vec3 rgbSE = FxaaTexOff(tex, pos.xy, vec2( 1.0,  1.0), rcpFrame).xyz;
    rgbL += (rgbNW + rgbNE + rgbSW + rgbSE);
    rgbL *= (1.0 / 9.0);
    
    mediump float lumaNW = FxaaLuma(rgbNW);
    mediump float lumaNE = FxaaLuma(rgbNE);
    mediump float lumaSW = FxaaLuma(rgbSW);
    mediump float lumaSE = FxaaLuma(rgbSE);
    
    mediump float edgeVert = 
        abs((0.25 * lumaNW) + (-0.5 * lumaN) + (0.25 * lumaNE)) +
        abs((0.50 * lumaW ) + (-1.0 * lumaM) + (0.50 * lumaE )) +
        abs((0.25 * lumaSW) + (-0.5 * lumaS) + (0.25 * lumaSE));
    mediump float edgeHorz = 
        abs((0.25 * lumaNW) + (-0.5 * lumaW) + (0.25 * lumaSW)) +
        abs((0.50 * lumaN ) + (-1.0 * lumaM) + (0.50 * lumaS )) +
        abs((0.25 * lumaNE) + (-0.5 * lumaE) + (0.25 * lumaSE));
        
    bool horzSpan = edgeHorz >= edgeVert;
    mediump float lengthSign = horzSpan ? -rcpFrame.y : -rcpFrame.x;
    
    if(!horzSpan)
    {
        lumaN = lumaW;
        lumaS = lumaE;
    }
    
    mediump float gradientN = abs(lumaN - lumaM);
    mediump float gradientS = abs(lumaS - lumaM);
    lumaN = (lumaN + lumaM) * 0.5;
    lumaS = (lumaS + lumaM) * 0.5;
    
    if (gradientN < gradientS)
    {
        lumaN = lumaS;
        lumaN = lumaS;
        gradientN = gradientS;
        lengthSign *= -1.0;
    }
    
    mediump vec2 posN;
    posN.x = pos.x + (horzSpan ? 0.0 : lengthSign * 0.5);
    posN.y = pos.y + (horzSpan ? lengthSign * 0.5 : 0.0);
    
    gradientN *= FXAA_SEARCH_THRESHOLD;
    
    mediump vec2 posP = posN;
    mediump vec2 offNP = horzSpan ? vec2(rcpFrame.x, 0.0) : vec2(0.0, rcpFrame.y); 
    mediump float lumaEndN = lumaN;
    mediump float lumaEndP = lumaN;
    bool doneN = false;
    bool doneP = false;
    posN += offNP * vec2(-1.0, -1.0);
    posP += offNP * vec2( 1.0,  1.0);
    
    for(int i = 0; i < FXAA_SEARCH_STEPS; ++i) {
        if(!doneN)
        {
            lumaEndN = FxaaLuma(texture2D(tex, posN.xy).xyz);
        }
        if(!doneP)
        {
            lumaEndP = FxaaLuma(texture2D(tex, posP.xy).xyz);
        }

        doneN = doneN || (abs(lumaEndN - lumaN) >= gradientN);
        doneP = doneP || (abs(lumaEndP - lumaN) >= gradientN);

        if(doneN && doneP)
        {
            break;
        }
        if(!doneN)
        {
            posN -= offNP;
        }
        if(!doneP)
        {
            posP += offNP;
        }
    }
    
    mediump float dstN = horzSpan ? pos.x - posN.x : pos.y - posN.y;
    mediump float dstP = horzSpan ? posP.x - pos.x : posP.y - pos.y;
    bool directionN = dstN < dstP;
    lumaEndN = directionN ? lumaEndN : lumaEndP;
    
    if(((lumaM - lumaN) < 0.0) == ((lumaEndN - lumaN) < 0.0))
    {
        lengthSign = 0.0;
    }
 

    mediump float spanLength = (dstP + dstN);
    dstN = directionN ? dstN : dstP;
    mediump float subPixelOffset = (0.5 + (dstN * (-1.0/spanLength))) * lengthSign;
    mediump vec3 rgbF = texture2D(tex, vec2(
        pos.x + (horzSpan ? 0.0 : subPixelOffset),
        pos.y + (horzSpan ? subPixelOffset : 0.0))).xyz;
    return FxaaLerp3(rgbL, rgbF, blendL); 
}

in highp vec2 vUV0;
void main()
{
    FragColor = vec4(SafeHDR(FxaaPixelShader(vUV0, RT, TexelSize0)), 1.0);
}
