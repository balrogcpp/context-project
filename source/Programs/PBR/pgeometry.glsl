// created by Andrey Vasiliev

#ifndef PGEOMETRY_GLSL
#define PGEOMETRY_GLSL

#include "noise.glsl"


// Apply grass waving animation to vertex
//----------------------------------------------------------------------------------------------------------------------
highp vec4 WaveGrass(const highp vec4 position, const highp float time, const float frequency, const vec4 direction)
{
    highp float n = NoiseHp(position.xz * time) * 2.0 - 2.0;
    return n * direction;
}


//----------------------------------------------------------------------------------------------------------------------
// Apply tree waving animation to vertex
highp vec4 WaveTree(const highp vec4 position, const highp float time, const vec4 params1, const vec4 params2)
{
    highp float radiusCoeff = params1.x;
    highp float radiusCoeff2 = radiusCoeff * radiusCoeff;
    highp float heightCoeff = params1.y;
    highp float heightCoeff2 = heightCoeff * heightCoeff;
    highp float factorX = params1.z;
    highp float factorY = params1.w;

    return vec4(
        sin(time + params2.z) * heightCoeff2 * factorX,
        sin(time + params2.z + position.y + position.x) * radiusCoeff2 * factorY,
        0.0,
        0.0
        );
}

#endif // PGEOMETRY_GLSL
