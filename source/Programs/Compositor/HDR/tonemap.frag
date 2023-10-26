// created by Andrey Vasiliev

#include "header.glsl"
#include "tonemap.glsl"

uniform sampler2D RT;

in vec2 vUV0;
void main()
{
    vec3 color = texture2D(RT, vUV0).rgb;
    color *= 2.0;
    FragColor.rgb = SafeHDR(unreal(color));
}
