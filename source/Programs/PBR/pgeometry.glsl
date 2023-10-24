// created by Andrey Vasiliev

#ifndef PGEOMETRY_GLSL
#define PGEOMETRY_GLSL

highp float Hash(highp float x)
{
    return fract(sin(x) * 43758.5453);
}

highp float Noise(highp vec2 x)
{
    highp vec2 p = floor(x);
    highp vec2 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);
    highp float n = p.x + p.y * 57.0;
    return mix(mix(Hash(n + 0.0), Hash(n + 1.0),f.x), mix(Hash(n + 57.0), Hash(n + 58.0), f.x), f.y);
}

// Apply grass waving animation to vertex
highp vec4 WaveGrass(highp vec4 position, highp float time, float frequency, vec4 direction)
{
    highp float n = Noise(position.xz * time) * 2.0 - 2.0;
    return n * direction;
}

// Apply tree waving animation to vertex
highp vec4 WaveTree(highp vec4 position, highp float time, vec4 params1, vec4 params2)
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
