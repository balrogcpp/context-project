// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform vec2 TexelSize;
uniform vec4 ViewportSize;

in highp vec2 vUV0;
void main()
{
    FragColor.rgb = texture2D(RT, vUV0).rgb;
}
