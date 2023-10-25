// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D FBO;
uniform int LightCount;
uniform vec4 LightPositionViewSpace[MAX_LIGHTS];
uniform int RayCount;
uniform float Decay;
uniform float Density;
uniform float Weight;
uniform float Exposure;

vec3 GodRays(sampler2D tex, vec2 uv, vec2 lightPos, int counter, float density, float weight, float decay, float exposure)
{
    vec3 color = vec3(0.0, 0.0, 0.0);
    vec2 suv = uv;
    float illuminationDecay = 1.0;
    vec2 deltaTextCoord = (uv - lightPos) * (density / float(counter));
    
    #define MAX_RAYS 100

    for(int i = 0; i < MAX_RAYS; ++i) {
        if (counter <= i) break;

        suv -= deltaTextCoord;
        color += texture2D(tex, suv).rgb * (illuminationDecay * weight);
        illuminationDecay *= decay;
    }

    color *= exposure;

    return color;
}

//in vec2 vUV0;
void main()
{
    vec3 color = vec3(0.0, 0.0, 0.0);
    vec2 uv = gl_FragCoord.xy / vec2(textureSize(RT, 0));

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (LightCount <= i) break;

        vec4 point = LightPositionViewSpace[i];
        float w = Weight * point.w;
        color += GodRays(FBO, uv, point.xy, RayCount, Density, w, Decay, Exposure);
    }

    FragColor.rgb = SafeHDR(color);
}
