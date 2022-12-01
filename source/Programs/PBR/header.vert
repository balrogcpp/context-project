// created by Andrey Vasiliev

#ifndef HEADER_VERT
#define HEADER_VERT

#ifdef GL_ES
#if __VERSION__ < 300
#extension GL_OES_standard_derivatives : enable
#endif
#endif


#ifndef GL_ES

#define precision
#define highp
#define mediump
#define lowp

#if __VERSION__ >= 150
#define attribute in
#define varying out
#else // __VERSION__ < 150
#define in attribute
#define out varying
#endif // __VERSION__ > 150

#else // GL_ES

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
