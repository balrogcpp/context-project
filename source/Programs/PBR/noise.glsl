// created by Andrey Vasiliev

#ifndef NOISE_GLSL
#define NOISE_GLSL


//----------------------------------------------------------------------------------------------------------------------
mediump float GoldNoise(const mediump vec2 xy, const mediump float seed)
{
    #define PHI 1.61803398874989484820459  //  PHI = Golden Ratio

    return fract(tan(distance(xy * PHI, xy) * seed) * xy.x);
}

//----------------------------------------------------------------------------------------------------------------------
highp float GoldNoiseHp(const highp vec2 xy, const highp float seed)
{
    #define PHI 1.61803398874989484820459  //  PHI = Golden Ratio

    return fract(tan(distance(xy * PHI, xy) * seed) * xy.x);
}

//----------------------------------------------------------------------------------------------------------------------
mediump float Random(const mediump vec2 x)
{
    return fract(sin(dot(x, vec2(12.9898, 78.233))) * 43758.5453);
}

//----------------------------------------------------------------------------------------------------------------------
highp float RandomHp(const highp vec2 x)
{
    return fract(sin(dot(x, vec2(12.9898, 78.233))) * 43758.5453);
}

//----------------------------------------------------------------------------------------------------------------------
mediump float BetterRandom(const mediump vec2 x)
{
    mediump float dt = dot(x, vec2(12.9898, 78.233));
    mediump float sn = mod(dt, 3.14159265359); // M_PI
    return fract(sin(sn) * 43758.5453);
}

//----------------------------------------------------------------------------------------------------------------------
highp float BetterRandomHp(const highp vec2 x)
{
    highp float dt = dot(x, vec2(12.9898, 78.233));
    highp float sn = mod(dt, 3.14159265359); // M_PI
    return fract(sin(sn) * 43758.5453);
}

//----------------------------------------------------------------------------------------------------------------------
mediump float InterleavedGradientNoise(const mediump vec2 position_screen)
{
    mediump vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * fract(dot(position_screen, magic.xy)));
}

//----------------------------------------------------------------------------------------------------------------------
highp float InterleavedGradientNoiseHp(const highp vec2 position_screen)
{
    highp vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * fract(dot(position_screen, magic.xy)));
}

//----------------------------------------------------------------------------------------------------------------------
mediump vec2 VogelDiskSample(const mediump float sampleIndex, const mediump float samplesCount, const mediump float phi)
{
    #define GoldenAngle 2.4

    mediump float r = sqrt((sampleIndex + 0.5) / samplesCount);
    mediump float theta = sampleIndex * GoldenAngle + phi;
    mediump float sine = sin(theta);
    mediump float cosine = cos(theta);
    return vec2(r * cosine, r * sine);
}

//----------------------------------------------------------------------------------------------------------------------
// Interleaved gradient function from Jimenez 2014
// http://www.iryoku.com/next-generation-post-processing-in-call-of-duty-advanced-warfare
mediump float GradientNoise(const mediump vec2 xy, const mediump vec2 params)
{
    mediump vec2 uv = floor(xy * params.xy);
    mediump float f = dot(vec2(0.06711056, 0.00583715), uv);
    return fract(52.9829189 * fract(f));
}

//----------------------------------------------------------------------------------------------------------------------
mediump float Hash(const mediump float x)
{
    return fract(sin(x) * 43758.5453);
}

//----------------------------------------------------------------------------------------------------------------------
highp float HashHp(const highp float x)
{
    return fract(sin(x) * 43758.5453);
}

//----------------------------------------------------------------------------------------------------------------------
mediump float Noise(const mediump vec2 x)
{
    mediump vec2 p = floor(x);
    mediump vec2 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);
    mediump float n = p.x + p.y * 57.0;
    return mix(mix(Hash(n + 0.0), Hash(n + 1.0),f.x), mix(Hash(n + 57.0), Hash(n + 58.0), f.x), f.y);
}

//----------------------------------------------------------------------------------------------------------------------
highp float NoiseHp(const highp vec2 x)
{
    highp vec2 p = floor(x);
    highp vec2 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);
    highp float n = p.x + p.y * 57.0;
    return mix(mix(HashHp(n + 0.0), HashHp(n + 1.0),f.x), mix(HashHp(n + 57.0), HashHp(n + 58.0), f.x), f.y);
}

//----------------------------------------------------------------------------------------------------------------------
mediump float Fbm(const mediump vec2 x)
{
    mediump float f = 0.0;
    mediump vec2 p = x;
    f += 0.50000 * Noise(p); p *= 2.02;
    f += 0.25000 * Noise(p); p *= 2.03;
    f += 0.12500 * Noise(p); p *= 2.01;
    f += 0.06250 * Noise(p); p *= 2.04;
    f += 0.03125 * Noise(p);
    return f / 0.984375;
}

//----------------------------------------------------------------------------------------------------------------------
highp float FbmHp(const highp vec2 x)
{
    highp float f = 0.0;
    highp vec2 p = x;
    f += 0.50000 * NoiseHp(p); p *= 2.02;
    f += 0.25000 * NoiseHp(p); p *= 2.03;
    f += 0.12500 * NoiseHp(p); p *= 2.01;
    f += 0.06250 * NoiseHp(p); p *= 2.04;
    f += 0.03125 * NoiseHp(p);
    return f / 0.984375;
}

#endif //NOISE_GLSL
