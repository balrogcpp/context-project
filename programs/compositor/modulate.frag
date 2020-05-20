#ifndef GL_ES
#define VERSION 120
#version VERSION
#define USE_TEX_LOD
#if VERSION != 120
#define texture1D texture
#define texture2D texture
#define texture2DProj textureProj
#define shadow2DProj textureProj
#define texture3D texture
#define textureCube texture
#define texture2DLod textureLod
#define textureCubeLod textureLod
#else
#define in varying
#define out varying
#endif
#ifdef USE_TEX_LOD
#extension GL_ARB_shader_texture_lod : require
#endif
#else
#version 100
#extension GL_OES_standard_derivatives : enable
#extension GL_EXT_shader_texture_lod: enable
#define textureCubeLod textureLodEXT
precision highp float;
#define in varying
#define out varying
#endif

#if VERSION != 120
out vec4 gl_FragColor;
#endif

in vec2 oUv0;
uniform float exposure;
uniform vec3 Ambient;
uniform vec4 screenSize;
uniform vec3 FogColour;
uniform vec4 FogParams;
uniform float NearClipDistance;
uniform float FarClipDistance;
uniform sampler2D AttenuationSampler;
uniform sampler2D SceneSampler;
uniform sampler2D MrtSampler;
uniform vec4 ShadowColour;

void main()
{
    vec4 tmp = texture2D(AttenuationSampler, oUv0);
    vec3 bloom = tmp.rgb;
//    float shadow = tmp.a;
    vec3 scene = texture2D(SceneSampler, oUv0).rgb;
    float depth = FarClipDistance * texture2D(MrtSampler, oUv0).r + NearClipDistance;

    float fog_value = 0.0;

    if (depth < 500.0) {
//        scene *= shadow;
        scene += bloom;
        float exponent = depth * FogParams.x;
        fog_value = 1.0 - clamp(1.0 / exp(exponent), 0.0, 1.0);
        scene = mix(scene, FogColour, fog_value);
    }

    gl_FragColor = vec4(scene, 1.0);
}