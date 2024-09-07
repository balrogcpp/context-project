// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

// Standard box filtering
vec3 UpsampleBox(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
    vec4 d = tsize.xyxy * vec4(-1.0, -1.0, 1.0, 1.0) * 0.5;

    vec3 c;
    c =  textureLod(tex, uv + d.xy, 0.0).rgb;
    c += textureLod(tex, uv + d.zy, 0.0).rgb;
    c += textureLod(tex, uv + d.xw, 0.0).rgb;
    c += textureLod(tex, uv + d.zw, 0.0).rgb;

    return c * 0.25;
}

void main()
{
    vec2 tsize = 1.0 / vec2(textureSize(RT, 0));
    vec2 uv = (0.5 * gl_FragCoord.xy) / vec2(textureSize(RT, 0));

    FragColor.rgb = UpsampleBox(RT, uv, tsize);
}
