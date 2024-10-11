// created by Andrey Vasiliev

#define HAS_MRT
#include "header.glsl"
#include "fog.glsl"
#include "srgb.glsl"

uniform sampler2D texTex;
uniform highp vec3 CameraPosition;
uniform vec3 LightDir0;
uniform vec4 FogColour;
uniform vec4 FogParams;
uniform float FarClipDistance;
uniform float NearClipDistance;

in highp vec3 vPosition;
in vec4 oUV;
in vec4 oColour;
out vec4 FragColor;
void main()
{
    vec4 s = texture(texTex, oUV.xy);
    vec3 color = s.rgb;
    float alpha = s.a;
    float fogCoord = gl_FragCoord.z / gl_FragCoord.w;

#ifdef ALPHA_TEST
    if(alpha < 0.5 || oColour.a < 0.5) discard;
#endif

    color = ApplyFog(color, FogParams.x, FogColour.rgb, fogCoord, normalize(CameraPosition - vPosition), LightDir0.xyz, CameraPosition);

    FragColor = vec4(color, alpha);
}
