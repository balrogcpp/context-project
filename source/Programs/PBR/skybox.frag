// created by Andrey Vasiliev

#define HAS_MRT
#include "header.glsl"
#include "srgb.glsl"
#include "fog.glsl"
#include "skymodel.glsl"
#ifdef FORCE_TONEMAP
#include "tonemap.glsl"
#endif

uniform vec3 LightColor0;
uniform vec3 LightDir0;
uniform float SunSize;
uniform vec4 FogColour;
uniform vec4 FogParams;
uniform vec3 HosekParams[10];
uniform float Time;
uniform float FarClipDistance;
uniform float NearClipDistance;

in highp vec3 vUV0;
void main()
{
    vec3 color = HosekWilkie(vUV0, -LightDir0.xyz, HosekParams);
    //vec3 color = sky(vUV0, N);

    highp vec3 V = normalize(vUV0);
    vec3 N = normalize(-LightDir0.xyz);
    float sunZenith = V.y;
    float cos_gamma = dot(V, N);
    float gamma = acos(cos_gamma);

    if (gamma <= M_PI / 360.0 && sunZenith > 0.0) {
        color = mix(color, LightColor0 * 10.0, 0.1);
    }

    // Apply exposure.
//    color += starfield(vUV0, -LightDir0, Time);
//    color = clouds(vUV0, color, LightColor0, -LightDir0, Time);
    color = SRGBtoLINEAR(color);

    // Fog
    color = ApplyFog(color, FogParams.x, FogColour.rgb, 200.0 * pow8(1.0 - V.y) - 25.0, V, LightDir0.xyz, vec3(0.0, 0.0, 0.0));

#ifdef FORCE_TONEMAP
    FragColor.rgb = SafeHDR(unreal(color));
#else 
    FragColor.rgb = SafeHDR(color);
#endif
    FragData[MRT_DEPTH].r = 0.999;
}
