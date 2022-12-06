// created by Andrey Vasiliev

#ifndef NOISE_GLSL
#define NOISE_GLSL


// https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
//const uint k = 1103515245U;  // GLIB C
//vec3 uhash3( uvec3 x )
//{
//    x = ((x>>8U)^x.yzx)*k;
//    x = ((x>>8U)^x.yzx)*k;
//    x = ((x>>8U)^x.yzx)*k;
//
//    return vec3(x)*(1.0/float(0xffffffffU));
//}


//----------------------------------------------------------------------------------------------------------------------
float new_random(vec2 x)
{
    float dt = dot(x, vec2(12.9898, 78.233));
    float sn = mod(dt, 3.14159265359); //  M_PI
    return fract(sin(sn) * 43758.5453);
}

//----------------------------------------------------------------------------------------------------------------------
float gold_noise(const vec2 xy, const float seed)
{
    #define PHI 1.61803398874989484820459  //  PHI = Golden Ratio

    return fract(tan(distance(xy * PHI, xy) * seed) * xy.x);
}

//----------------------------------------------------------------------------------------------------------------------
float random(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

//----------------------------------------------------------------------------------------------------------------------
float InterleavedGradientNoise(vec2 position_screen)
{
    vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * fract(dot(position_screen, magic.xy)));
}

//----------------------------------------------------------------------------------------------------------------------
vec2 VogelDiskSample(int sampleIndex, int samplesCount, float phi)
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
float GradientNoise(vec2 uv, vec2 params)
{
    uv = floor(uv * params.xy);
    float f = dot(vec2(0.06711056, 0.00583715), uv);
    return fract(52.9829189 * fract(f));
}

//----------------------------------------------------------------------------------------------------------------------
float hash(float n)
{
    return fract(sin(n) * 43758.5453);
}

//----------------------------------------------------------------------------------------------------------------------
float noise(vec2 x)
{
    vec2 p = floor(x);
    vec2 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);
    float n = p.x + p.y * 57.0;
    float res = mix(mix( hash(n + 0.0), hash(n + 1.0),f.x), mix(hash(n + 57.0), hash(n + 58.0), f.x), f.y);
    return res;
}

//----------------------------------------------------------------------------------------------------------------------
float fbm(vec2 p)
{
    //float f = 0.0;
    //f += 0.50000 * noise(p); p = p * 2.02;
    //  f += 0.25000 * noise(p); p = p * 2.03;
    //  f += 0.12500 * noise(p); p = p * 2.01;
    //  f += 0.06250 * noise(p); p = p * 2.04;
    //  f += 0.03125 * noise(p);
    //return f / 0.984375;
    return noise(p);
}

#endif //NOISE_GLSL
