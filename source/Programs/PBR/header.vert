// created by Andrey Vasiliev

#ifndef HEADER_VERT
#define HEADER_VERT

#ifndef GL_ES

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
