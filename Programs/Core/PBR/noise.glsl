// This source file is part of "glue project". Created by Andrew Vasiliev

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

    float r = sqrt(float(sampleIndex) + 0.5) / sqrt(float(samplesCount));
    float theta = float(sampleIndex) * GoldenAngle + phi;
    float sine = sin(theta);
    float cosine = cos(theta);

    return vec2(r * cosine, r * sine);
}

#endif //NOISE_GLSL
