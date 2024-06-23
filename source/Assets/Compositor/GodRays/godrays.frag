// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D FBO;
uniform int LightCount;
uniform vec4 LightPosition;
uniform float Decay;
uniform float Density;
uniform float Weight;
uniform float Exposure;

#define RAYS_NUMBER 100

vec3 GodRays(sampler2D tex, const vec2 uv, const vec2 lightPos, float density, float weight, float decay, float exposure)
{
    highp vec3 color = vec3(0.0, 0.0, 0.0);
    highp vec2 suv = uv;
    highp float illuminationDecay = 1.0;
    highp vec2 deltaTextCoord = (uv - lightPos) * (density / float(RAYS_NUMBER));

    for(int i = 0; i < RAYS_NUMBER; ++i) {
        suv -= deltaTextCoord;
        color += texture2D(tex, suv).rgb * (illuminationDecay * weight);
        illuminationDecay *= decay;
    }

    color *= exposure;

    return color;
}

in highp vec2 vUV0;
void main()
{
    vec3 color = GodRays(FBO, vUV0, LightPosition.xy, Density, Weight * LightPosition.w, Decay, Exposure);

    FragColor.rgb = color;
}
