// This source file is part of "glue project". Created by Andrew Vasiliev

#ifndef HEADER_VERT
#define HEADER_VERT

#ifndef GL_ES

// GLSL

#if VERSION != 120
#define attribute in
#define varying out
#else
#define in attribute
#define out varying
#endif

#else

// GLSLES 2.0

#extension GL_OES_standard_derivatives : enable
#define textureCubeLod textureLodEXT
precision highp float;
precision lowp int;
precision lowp sampler2D;
precision lowp samplerCube;
#if VERSION == 100
#define in attribute
#define out varying
#else
#define attribute in
#endif
#endif

#endif //HEADER_VERT
