// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
//uniform sampler2D DepthTex;

in highp vec2 vUV0;
void main()
{
    vec3 color = texture2D(RT, vUV0).rgb;
//    float depth = texture2D(DepthTex, vUV0).r;
    float lum = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if (lum > 10.0) lum = 0.0;
    lum = max(0.0, lum - 1.0);
    FragColor.rgb = SafeHDR(color * sign(lum));
//    FragColor.rgb = depth < 0.8 ? SafeHDR(color * sign(lum)) : vec3(0.0, 0.0, 0.0);
}
