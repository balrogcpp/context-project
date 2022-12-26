// created by Andrey Vasiliev

#ifndef HEADER_VERT
#define HEADER_VERT

#ifndef MAX_LIGHTS
#define MAX_LIGHTS 8
#endif

#ifndef MAX_SHADOW_TEXTURES
#define MAX_SHADOW_TEXTURES 4
#endif

#ifdef GL_ES
#if __VERSION__ < 300
#extension GL_OES_standard_derivatives : enable
#endif
#endif


#ifndef GL_ES

#if __VERSION__ >= 150
#define attribute in
#define varying out
#else // __VERSION__ < 150
#define in attribute
#define out varying
#endif // __VERSION__ > 150

#else

precision highp float;
precision lowp int;
precision lowp sampler2D;
precision lowp samplerCube;
#if __VERSION__ >= 300
#define attribute in
#define varying out
#else // __VERSION__ < 300
#define in attribute
#define out varying
#endif // __VERSION__ >= 300
#endif // GLSL_ES

#endif //HEADER_VERT
