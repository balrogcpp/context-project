// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D FBO;
uniform int LightCount;
uniform vec4 LightPositionList[MAX_LIGHTS];
uniform float Decay;
uniform float Density;
uniform float Weight;
uniform float Exposure;

vec3 GodRays(vec2 uv, const vec4 lightPos)
{
    #define MAX_RAYS 50

    vec3 color = vec3(0.0, 0.0, 0.0);
    float illuminationDecay = 0.5;
    vec2 deltaTexCoord = (uv - lightPos.xy) * (Density / float(MAX_RAYS));
    
    for(int i = 0; i < MAX_RAYS; ++i) {
        uv -= deltaTexCoord;
        color += texture2D(FBO, uv).rgb * (illuminationDecay * Weight * lightPos.w);
        illuminationDecay *= Decay;
    }

    color *= Exposure;

    return color;
}

in highp vec2 vUV0;
void main()
{
    vec3 color = vec3(0.0, 0.0, 0.0);

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (LightCount <= i) break;

        color += GodRays(vUV0, LightPositionList[i]);
    }

    FragColor.rgb = SafeHDR(color);
}
