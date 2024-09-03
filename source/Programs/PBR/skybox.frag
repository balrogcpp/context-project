// created by Andrey Vasiliev

#define HAS_MRT
#include "header.glsl"
#include "mrt.glsl"
#include "math.glsl"
#include "srgb.glsl"
#include "fog.glsl"
#include "skymodel.glsl"
//#include "clouds.glsl"

uniform vec3 LightColor0;
uniform vec3 LightDir0;
uniform float SunSize;
uniform vec4 FogColour;
uniform vec4 FogParams;
uniform vec3 HosekParams[10];
uniform float Time;

//------------------------------------------------------------------------------
// Trigonometry
//------------------------------------------------------------------------------

/**
 * Approximates acos(x) with a max absolute error of 9.0x10^-3.
 * Valid in the range -1..1.
 */
float acosFast(float x) {
    // Lagarde 2014, "Inverse trigonometric functions GPU optimization for AMD GCN architecture"
    // This is the approximation of degree 1, with a max absolute error of 9.0x10^-3
    float y = abs(x);
    float p = -0.1565827 * y + 1.570796;
    p *= sqrt(1.0 - y);
    return x >= 0.0 ? p : PI - p;
}

/**
 * Approximates acos(x) with a max absolute error of 9.0x10^-3.
 * Valid only in the range 0..1.
 */
float acosFastPositive(float x) {
    float p = -0.1565827 * x + 1.570796;
    return p * sqrt(1.0 - x);
}

in highp vec3 vUV0;
void main()
{
    vec3 color = HosekWilkie(vUV0, -LightDir0.xyz, HosekParams);

    highp vec3 V = normalize(vUV0);
    vec3 N = normalize(-LightDir0.xyz);
    float sunZenith = V.y;
    float cos_gamma = dot(V, N);
    float gamma = acosFastPositive(cos_gamma);

    // Apply exposure.
    color = SRGBtoLINEAR(color);

    if (gamma <= 2.0 * PI / 360.0 && sunZenith > 0.0) {
        color = LightColor0 * 100.0;
    }

//    color = clouds(vUV0, color, LightColor0, -LightDir0, 0.0);

    // Fog
//    color = ApplyFog(color, FogParams.x, FogColour.rgb, 200.0 * pow8(1.0 - V.y) - 25.0, V, LightDir0.xyz, vec3(0.0, 0.0, 0.0));
    color = ApplyFog(color, FogParams.x, FogColour.rgb, 100.0, V, -LightDir0.xyz, vec3(0.0, 0.0, 0.0));

    FragColor = SafeHDR(vec4(color, 1.0));
}
