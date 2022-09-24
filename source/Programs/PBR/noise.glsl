/// created by Andrey Vasiliev

#ifndef NOISE_GLSL
#define NOISE_GLSL

//----------------------------------------------------------------------------------------------------------------------
float InterleavedGradientNoise(vec2 position_screen)
{
    vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * fract(dot(position_screen, magic.xy)));
}

//----------------------------------------------------------------------------------------------------------------------
vec2 VogelDiskSample(int sampleIndex, int samplesCount, float phi)
{
    const float GoldenAngle = 2.4;

    float r = sqrt((float(sampleIndex) + 0.5) / float(samplesCount));
    float theta = float(sampleIndex) * GoldenAngle + phi;
    float sine = sin(theta);
    float cosine = cos(theta);

    return vec2(r * cosine, r * sine);
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
