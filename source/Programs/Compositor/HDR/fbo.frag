// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D Lum;

uniform vec2 brightThreshold;

in highp vec2 vUV0;
void main()
{
    float lum = texelFetch(Lum, ivec2(0, 0), 0).r;
    vec3 color = textureLod(RT, vUV0, 0.0).rgb * lum;
    
    vec3 w = clamp((color - brightThreshold.x) * brightThreshold.y, 0.0, 1.0);
    color *= w * w * (3.0 - 2.0 * w);

    FragColor.rgb = color;
}
