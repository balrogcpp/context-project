// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform vec4 ViewportSize;

void main()
{
    FragColor.rgb = texture2D(RT, gl_FragCoord.xy * ViewportSize.zw).rgb;
}
