// This source file is part of "glue project". Created by Andrey Vasiliev

#ifndef HEADER_VERT
#define HEADER_VERT

#ifndef GL_ES

#if VERSION == 330 || VERSION == 400 || VERSION == 410 || VERSION == 420 || VERSION == 430 || VERSION == 440 || VERSION == 450 || VERSION == 460
#define attribute in
#define varying out
#else
#define in attribute
#define out varying
#endif

#else

#extension GL_OES_standard_derivatives : enable
#define textureCubeLod textureLodEXT
precision highp float;
precision lowp int;
precision lowp sampler2D;
precision lowp samplerCube;
#if VERSION == 300 || VERSION == 310 || VERSION == 320
#define attribute in
#else
#define in attribute
#define out varying
#endif

#endif // GLSL_ES

#endif //HEADER_VERT
