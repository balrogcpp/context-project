// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D FBO;

in highp vec2 vUV0;
void main()
{
    vec3 color = texture2D(RT, vUV0).rgb;
    vec3 fbo = texture2D(FBO, vUV0).rgb;

    FragColor.rgb = SafeHDR(color + fbo / 12.0);
}
