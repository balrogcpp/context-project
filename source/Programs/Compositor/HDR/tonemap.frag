// created by Andrey Vasiliev

#include "header.glsl"
#include "tonemap.glsl"

uniform sampler2D RT;
uniform float Exposure;

in vec2 vUV0;
void main()
{
    vec3 color = texture2D(RT, vUV0).rgb;
    color = expose(color, Exposure);
    FragColor.rgb = SafeHDR(unreal(color));
}
