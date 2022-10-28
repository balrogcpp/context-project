// created by Andrey Vasiliev

#ifndef HEADER_VERT
#define HEADER_VERT

#ifndef GL_ES

#if VERSION == 330 || VERSION == 400 || VERSION == 410 || VERSION == 420 || VERSION == 430 || VERSION == 440 || VERSION == 450 || VERSION == 460
#define attribute in
#define varying out
#else // VERSION < 330
#define in attribute
#define out varying
#endif // VERSION >= 330

#else // GL_ES

#extension GL_OES_standard_derivatives : enable

precision highp float;
precision lowp int;
precision lowp sampler2D;
precision lowp samplerCube;

#if VERSION == 300 || VERSION == 310 || VERSION == 320
#define attribute in
#define varying out
#else // VERSION < 300
#define in attribute
#define out varying
#endif // VERSION >= 300

#endif // GLSL_ES

#endif //HEADER_VERT
