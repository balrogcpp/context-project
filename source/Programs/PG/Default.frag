// created by Andrey Vasiliev

#define HAS_MRT
#include "header.glsl"
#include "fog.glsl"
#include "srgb.glsl"

uniform sampler2D texTex;
uniform vec4 FogColour;
uniform vec4 FogParams;

in vec4 oUV;
in vec4 oColour;
//in float oFogCoord;
void main()
{
    vec4 s = texture2D(texTex, oUV.xy);
    vec3 color = s.rgb;
    float alpha = s.a;

#ifdef ALPHA_TEST
    if(alpha < 0.5 || oColour.a < 0.5) discard;
#endif

    color = SRGBtoLINEAR(color);
    color = ApplyFog(color, FogParams.x, FogColour.rgb, gl_FragCoord.z / gl_FragCoord.w);
    //color = ApplyFog(color, FogParams.x, FogColour.rgb, gl_FragCoord.z / gl_FragCoord.w, vVec, WorldSpaceLightPos0.xyz, CameraPosition);

#ifdef FORCE_TONEMAP
    FragColor.rgb = SafeHDR(unreal(color));
#else 
    FragColor.rgb = SafeHDR(color);
#endif
    FragColor.a = alpha;
}
