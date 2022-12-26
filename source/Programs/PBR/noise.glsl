// created by Andrey Vasiliev

#ifndef NOISE_GLSL
#define NOISE_GLSL


//----------------------------------------------------------------------------------------------------------------------
float GoldNoise(const vec2 xy, const float seed)
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
float Random(const vec2 x)
{
    return fract(sin(dot(x, vec2(12.9898, 78.233))) * 43758.5453);
}

//----------------------------------------------------------------------------------------------------------------------
highp float RandomHp(const highp vec2 x)
{
    return fract(sin(dot(x, vec2(12.9898, 78.233))) * 43758.5453);
}

//----------------------------------------------------------------------------------------------------------------------
float BetterRandom(const vec2 x)
{
    float dt = dot(x, vec2(12.9898, 78.233));
    float sn = mod(dt, 3.14159265359); //  M_PI
    return fract(sin(sn) * 43758.5453);
}

//----------------------------------------------------------------------------------------------------------------------
highp float BetterRandomHp(const highp vec2 x)
{
    highp float dt = dot(x, vec2(12.9898, 78.233));
    highp float sn = mod(dt, 3.14159265359); //  M_PI
    return fract(sin(sn) * 43758.5453);
}

//----------------------------------------------------------------------------------------------------------------------
float InterleavedGradientNoise(const vec2 position_screen)
{
    vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * fract(dot(position_screen, magic.xy)));
}

//----------------------------------------------------------------------------------------------------------------------
highp float InterleavedGradientNoiseHp(const highp vec2 position_screen)
{
    highp vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * fract(dot(position_screen, magic.xy)));
}

//----------------------------------------------------------------------------------------------------------------------
vec2 VogelDiskSample(const int sampleIndex, const int samplesCount, const float phi)
{
    #define GoldenAngle 2.4

    float r = sqrt((float(sampleIndex) + 0.5) / float(samplesCount));
    float theta = float(sampleIndex) * GoldenAngle + phi;
    float sine = sin(theta);
    float cosine = cos(theta);

    return vec2(r * cosine, r * sine);
}

//----------------------------------------------------------------------------------------------------------------------
// Interleaved gradient function from Jimenez 2014
// http://www.iryoku.com/next-generation-post-processing-in-call-of-duty-advanced-warfare
float GradientNoise(const vec2 xy, const vec2 params)
{
    vec2 uv = floor(xy * params.xy);
    float f = dot(vec2(0.06711056, 0.00583715), uv);
    return fract(52.9829189 * fract(f));
}

//----------------------------------------------------------------------------------------------------------------------
float Hash(const float x)
{
    return fract(sin(x) * 43758.5453);
}

//----------------------------------------------------------------------------------------------------------------------
highp float HashHp(const highp float x)
{
    return fract(sin(x) * 43758.5453);
}

//----------------------------------------------------------------------------------------------------------------------
float Noise(const vec2 x)
{
    vec2 p = floor(x);
    vec2 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);
    float n = p.x + p.y * 57.0;
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
float Fbm(const vec2 x)
{
    float f = 0.0;
    vec2 p = x;
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
