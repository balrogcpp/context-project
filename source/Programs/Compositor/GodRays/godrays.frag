// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D FBO;
uniform int LightCount;
uniform vec4 LightPosition;
uniform int RayCount;
uniform float Decay;
uniform float Density;
uniform float Weight;
uniform float Exposure;

vec3 GodRays(sampler2D tex, const vec2 uv, const vec2 lightPos, int counter, float density, float weight, float decay, float exposure)
{
    vec3 color = vec3(0.0, 0.0, 0.0);
    vec2 suv = uv;
    float illuminationDecay = 1.0;
    vec2 deltaTextCoord = (uv - lightPos) * (density / float(counter));

    for(int i = 0; i < 100; ++i) {
        if (counter <= i) break;

        suv -= deltaTextCoord;
        color += texture2D(tex, suv).rgb * (illuminationDecay * weight);
        illuminationDecay *= decay;
    }

    color *= exposure;

    return color;
}

in vec2 vUV0;
void main()
{
    vec3 color = GodRays(FBO, vUV0, LightPosition.xy, RayCount, Density, LightPosition.w, Decay, Exposure);

    FragColor.rgb = SafeHDR(color);
}
