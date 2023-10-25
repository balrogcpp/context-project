// created by Andrey Vasiliev

#include "header.glsl"
#include "tonemap.glsl"

uniform sampler2D RT;
uniform sampler2D Lum;

void main()
{
    ivec2 uv = ivec2(gl_FragCoord.xy);
    vec3 color = texelFetch(RT, uv, 0).rgb;
    float lum = texture2D(Lum, vec2(0.0, 0.0)).r;

    color *= lum;
    FragColor.rgb = SafeHDR(unreal(color));
}
