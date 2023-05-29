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
#include "FastNoiseLite.glsl"

OGRE_UNIFORMS_BEGIN
uniform highp float Time;
OGRE_UNIFORMS_END

MAIN_PARAMETERS
IN(mediump vec2 vUV0, TEXCOORD0)
MAIN_DECLARATION
{
    fnl_state state = fnlCreateState(1337);
    state.noise_type = FNL_NOISE_OPENSIMPLEX2S;
    state.fractal_type = FNL_FRACTAL_RIDGED;
    state.frequency = 0.011;
    state.octaves = 5;
    state.lacunarity = 2.0;
    state.gain = 0.7;

    highp float noise = fnlGetNoise3D(state, gl_FragCoord.x, Time * 60.0, gl_FragCoord.y) * 0.5 + 0.5;

    FragColor = vec4(noise, 0.0, 0.0, 1.0);
}
