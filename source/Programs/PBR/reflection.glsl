// created by Andrey Vasiliev

#ifndef REFLECTION_GLSL
#define REFLECTION_GLSL

#include "noise.glsl"


//----------------------------------------------------------------------------------------------------------------------
vec4 GetProjectionCoord(const vec4 position) {
    return mat4(0.5, 0.0, 0.0, 0.0,
                0.0, 0.5, 0.0, 0.0,
                0.0, 0.0, 0.5, 0.0,
                0.5, 0.5, 0.5, 1.0
                ) * position;
}


#endif
