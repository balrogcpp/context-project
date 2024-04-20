// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

void main()
{
    vec2 uv = gl_FragCoord.xy * 2.0 / vec2(textureSize(RT, 0));
    vec3 color = texture2D(RT, uv).rgb;
    float lum = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if (lum > 20.0) lum = 0.0;
    lum = max(0.0, lum - 1.0);
    FragColor.rgb = color * sign(lum);
}
