// created by Andrey Vasiliev
//? #version 400

#include "fx.glsl"

uniform sampler2D RT;
uniform sampler2D LUT;
uniform float Time;


out vec3 FragColor;
void main()
{
    vec2 uv = gl_FragCoord.xy / vec2(textureSize(RT, 0));
    vec3 color = texelFetch(RT, ivec2(gl_FragCoord.xy), 0).rgb;

    color = applyPostEffects(color, uv, Time, LUT);

    FragColor.rgb = color;
}
