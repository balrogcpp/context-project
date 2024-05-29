// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

in highp vec2 vUV0;
void main()
{
    ivec2 uv = ivec2(gl_FragCoord.xy);

    vec3 color = texelFetch(RT, uv, 0).rgb;

    uv.x -= 100 / 2;
    for (int i = 1; i < 100; ++i) {
        uv.x++;
        color += texelFetch(RT, uv, 0).rgb;
    }

    // uv = ivec2(gl_FragCoord.xy);
    // uv.y -= 11 / 2;
    // for (int i = 1; i < 11; ++i) {
    //     uv.y++;
    //     color += texelFetch(RT, uv, 0).rgb;
    // }


    FragColor.rgb = color / float(100);
}
