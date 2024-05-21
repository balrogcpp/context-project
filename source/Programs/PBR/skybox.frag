// created by Andrey Vasiliev

#define HAS_MRT
#include "header.glsl"
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

in highp vec3 vUV0;
void main()
{
    vec3 color = HosekWilkie(vUV0, -LightDir0.xyz, HosekParams);

    highp vec3 V = normalize(vUV0);
    vec3 N = normalize(-LightDir0.xyz);
    float sunZenith = V.y;
    float cos_gamma = dot(V, N);
    float gamma = acos(cos_gamma);

    // Apply exposure.
    color = SRGBtoLINEAR(color);

    if (gamma <= 2.0 * PI / 360.0 && sunZenith > 0.0) {
        color = LightColor0 * 10.0;
    }

//    color = clouds(vUV0, color, LightColor0, -LightDir0, 0.0);

    // Fog
//    color = ApplyFog(color, FogParams.x, FogColour.rgb, 200.0 * pow8(1.0 - V.y) - 25.0, V, LightDir0.xyz, vec3(0.0, 0.0, 0.0));
    color = ApplyFog(color, FogParams.x, FogColour.rgb, 100.0, V, -LightDir0.xyz, vec3(0.0, 0.0, 0.0));

    EvaluateBuffer(vec4(color, 1.0), 0.999);
}
