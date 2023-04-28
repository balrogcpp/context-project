// created by Andrey Vasiliev

#ifndef __VERSION__
#ifndef GL_ES
#version 330 core
#define __VERSION__ 330
#else
#version 300 es
#define __VERSION__ 300
#endif
#endif


#include "header.glsl"
#include "math.glsl"


in highp vec4 vertex;

out highp vec3 vWorldPosition;
out mediump mat3 vTBN;
out mediump vec4 vScreenPosition;
out mediump vec4 vPrevScreenPosition;

uniform highp mat4 WorldMatrix;
uniform highp mat4 WorldViewProjMatrix;
uniform highp mat4 WorldViewProjPrev;
uniform highp vec4 CameraPosition;

uniform mediump vec4 Time;
uniform mediump float bumpScale;
uniform mediump vec2 textureScale;
uniform mediump vec2 bumpSpeed;
uniform mediump float waveFreq;
uniform mediump float waveAmp;


// wave functions
struct Wave {
    float freq;  // 2*PI / wavelength
    float amp;   // amplitude
    float phase; // speed * 2*PI / wavelength
    vec2 dir;
};

void main()
{
    #define NWAVES 2

    Wave wave[NWAVES];

    wave[0] = Wave( waveFreq, waveAmp, 0.5, vec2(-1.0, 0.0) );
    wave[1] = Wave( 3.0 * waveFreq, 0.33 * waveAmp, 1.7, vec2(-0.7, 0.7) );

    highp vec4 position = vertex;

    // sum waves
    highp float ddx = 0.0;
    highp float ddy = 0.0;
    highp float deriv = 0.0;
    highp float angle = 0.0;

    // wave synthesis using two sine waves at different frequencies and phase shift
//    for(int i = 0; i < NWAVES; ++i) {
//        angle = dot(wave[i].dir, position.xz) * wave[i].freq + Time.x * wave[i].phase;
//        position.y += wave[i].amp * sin( angle );
//        // calculate derivate of wave function
//        deriv = wave[i].freq * wave[i].amp * cos(angle);
//        ddx -= deriv * wave[i].dir.x;
//        ddy -= deriv * wave[i].dir.y;
//    }

    // compute the 3x3 transform from tangent space to object space
    // compute tangent basis
    highp vec3 t = normalize(vec3(1.0, ddy, 0.0)) * bumpScale;
    highp vec3 b = normalize(vec3(0.0, ddx, 1.0)) * bumpScale;
    highp vec3 n = normalize(vec3(ddx, 1.0, ddy));
    vTBN = mtxFromCols3x3(t, b, n);

    highp vec4 world = mul(WorldMatrix, position);
    vWorldPosition = world.xyz / world.w;
    gl_Position = mul(WorldViewProjMatrix, position);

    vScreenPosition = gl_Position;
    vPrevScreenPosition = mul(WorldViewProjPrev, position);
}
