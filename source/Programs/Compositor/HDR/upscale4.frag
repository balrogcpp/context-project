// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

// Standard box filtering
vec3 UpsampleBox(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
    vec4 d = tsize.xyxy * vec4(-1.0, -1.0, 1.0, 1.0) * 0.5;

    vec3 s;
    s =  textureLod(tex, uv + d.xy, 0.0).rgb;
    s += textureLod(tex, uv + d.zy, 0.0).rgb;
    s += textureLod(tex, uv + d.xw, 0.0).rgb;
    s += textureLod(tex, uv + d.zw, 0.0).rgb;

    return s * (1.0 / 4.0);
}

void main()
{
    vec2 size = vec2(textureSize(RT, 0) * 2.0);
    vec2 tsize = 1.0 / size;
    vec2 uv = gl_FragCoord.xy / size;
    uv.y = 1.0 - uv.y;

    FragColor.rgb = UpsampleBox(RT, uv, tsize);
}
