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

OGRE_UNIFORMS_BEGIN
uniform mediump vec2 TexelSize0;
OGRE_UNIFORMS_END

MAIN_PARAMETERS
IN(mediump vec2 vUV0, TEXCOORD0)
MAIN_DECLARATION
{
    const float SCALE = 11.0;
    highp float s11 = texture2D(RT, vUV0).x * SCALE;
    highp float s01 = texture2D(RT, vUV0 + TexelSize0 * vec2(-1.0, 0.0)).x * SCALE;
    highp float s21 = texture2D(RT, vUV0 + TexelSize0 * vec2(1.0, 0.0)).x * SCALE;
    highp float s10 = texture2D(RT, vUV0 + TexelSize0 * vec2(0.0, -1.0)).x * SCALE;
    highp float s12 = texture2D(RT, vUV0 + TexelSize0 * vec2(0.0, 1.0)).x * SCALE;
    highp vec3 va = normalize(vec3(2.0, 0.0, s21 - s01));
    highp vec3 vb = normalize(vec3(0.0, 2.0, s12 - s10));
    highp vec4 bump = vec4(cross(va, vb), s11);
    bump = bump * 0.5 + 0.5;

    FragColor = bump;
    // FragColor = vec4(s12);
}
