// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform vec4 ViewportSize;

out vec4 FragColor;
void main()
{
    vec2 uv = gl_FragCoord.xy * ViewportSize.zw;
    uv.y = 1.0 - uv.y;

    FragColor = textureLod(RT, uv, 0.0);
}
