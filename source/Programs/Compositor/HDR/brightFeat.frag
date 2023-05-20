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

#include "header.glsl"
SAMPLER2D(RT, 0);
uniform mediump vec2 TexelSize0;
uniform mediump float ChromaticRadius;
uniform mediump int FeaturesCount;


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 SampleChromatic(sampler2D tex, const mediump vec2 uv, const mediump float radius)
{
    mediump vec2 offset = normalize(vec2(0.5, 0.5) - uv) * radius;

    mediump vec3 color = vec3(
        texture2D(tex, uv + offset).r,
        texture2D(tex, uv).g,
        texture2D(tex, uv - offset).b
    );

    return color;
}


// https://john-chapman.github.io/2017/11/05/pseudo-lens-flare.html
// Cubic window; map [0, radius] in [1, 0] as a cubic falloff from center.
//----------------------------------------------------------------------------------------------------------------------
mediump float WindowCubic(const mediump float x, const mediump float center, const mediump float radius)
{
    mediump float y = min(abs(x - center) / radius, 1.0);
    return 1.0 - y * y * (3.0 - 2.0 * y);
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 GhostFeatures(sampler2D tex, const mediump vec2 uv, const mediump vec2 texel, const mediump int counter, const mediump float radius)
{
    mediump vec2 nuv = vec2(1.0, 1.0) - uv;
    mediump vec2 ghostVec = (vec2(0.5, 0.5) - nuv) * 0.44;
    mediump vec3 color = vec3(0.0, 0.0, 0.0);

    #define MAX_GHOST_COUNT 24

    // ghosts
    for (mediump int i = 0; i < MAX_GHOST_COUNT; ++i) {
        if (counter <= i) break;

        mediump vec2 suv = fract(nuv + ghostVec * float(i));
        mediump float d = distance(suv, vec2(0.5, 0.5));
        mediump float w = pow(1.0 - d, 5.0) / float(counter);
        mediump vec3 s = SampleChromatic(tex, suv, radius);
        color += s * w;
    }

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 HaloFeatures(sampler2D tex, const mediump vec2 uv, const mediump vec2 texel, const mediump int counter, const mediump float radius)
{
    mediump vec2 nuv = vec2(1.0, 1.0) - uv;
    mediump vec2 haloVec = vec2(0.5, 0.5) - nuv;

    #define RADIUS 0.45

    // halo
    mediump float ratio = texel.y / texel.x;
    haloVec.x *= ratio;
    haloVec = normalize(haloVec);
    haloVec.x /= ratio;
    mediump vec2 wuv = (nuv - vec2(0.5, 0.0)) / vec2(ratio, 1.0) + vec2(0.5, 0.0);
    mediump float d = distance(wuv, vec2(0.5, 0.5));
    mediump float w = pow(1.0 - d, 5.0);
    haloVec *= RADIUS;

    mediump vec2 suv = nuv + haloVec;
    mediump vec3 s = SampleChromatic(tex, suv, radius);
    mediump vec3 color = s * w;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
MAIN_PARAMETERS
IN(mediump vec2 vUV0, TEXCOORD0)

MAIN_DECLARATION
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;

    //color += GhostFeatures(RT, vUV0, TexelSize0, FeaturesCount, ChromaticRadius) * 0.5;
    color += HaloFeatures(RT, vUV0, TexelSize0, FeaturesCount, ChromaticRadius) * 0.5;

    FragColor = vec4(color, 1.0);
}
