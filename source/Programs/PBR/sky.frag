// created by Andrey Vasiliev

#define HAS_MRT
#include "header.glsl"
#include "srgb.glsl"
#include "fog.glsl"

uniform vec3 SunColor;
uniform highp vec4 LightDirection;
uniform float SunSize;
uniform vec4 FogColour;
uniform vec4 FogParams;

#ifdef GPU_HOSEK
//#include "hosek.glsl"
#else
uniform vec3 A;
uniform vec3 B;
uniform vec3 C;
uniform vec3 D;
uniform vec3 E;
uniform vec3 F;
uniform vec3 G;
uniform vec3 H;
uniform vec3 I;
uniform vec3 Z;

vec3 HosekWilkie(float cos_theta, float gamma, float cos_gamma)
{
    float cos_gamma2 = cos_gamma * cos_gamma;
    vec3 I2 = I * I;
    vec3 chi = ((1.0 + cos_gamma2) / pow(1.0 + I2 - 2.0 * (cos_gamma * I), vec3(1.5, 1.5, 1.5)));
    return Z * ((1.0 + A * exp(B / (cos_theta + 0.01))) * (C + D * exp(E * gamma) + F * cos_gamma2 + G * chi + H * sqrt(cos_theta)));
}
#endif


// Clamps color between 0 and 1 smoothly
vec3 SkyLightExpose(const vec3 color)
{
    return 2.0 / (1.0 + exp(-0.1 * color)) - 1.0;
}

in vec3 vUV0;
void main()
{
    vec3 V = normalize(vUV0);
    vec3 N = normalize(-LightDirection.xyz);
    float cos_theta = clamp(abs(V.y), 0.0, 1.0);
    float cos_gamma = dot(V, N);
    float gamma = acos(cos_gamma);
#ifdef GPU_HOSEK
    //vec3 color = sample_sky(cos_theta, gamma, N.y, N.x);
#else
    vec3 color = HosekWilkie(cos_theta, gamma, cos_gamma);
#endif

    if (gamma <= SunSize && V.y > 0.0) {
        color = SunColor;
    }

    color = SRGBtoLINEAR(SkyLightExpose(color));

    color = ApplyFog(color, FogParams.x, FogColour.rgb, 300.0 * pow(1 - V.y, 3.0), V, LightDirection.xyz, vec3(0.0, 0.0, 0.0));

    EvaluateBuffer(color);
}
