// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D FBO;

//in vec2 vUV0;
void main()
{
    vec2 uv = gl_FragCoord.xy / vec2(textureSize(RT, 0));
    vec3 color = texture2D(RT, uv).rgb;
    vec3 rays = texture2D(FBO, uv).rgb;

    FragColor.rgb = SafeHDR(color + rays);
}
