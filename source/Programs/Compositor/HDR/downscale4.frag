// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L43
vec3 DownsampleBox4Tap(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
    vec4 d = tsize.xyxy * vec4(-1.0, -1.0, 1.0, 1.0);

    vec3 c = textureLod(tex, uv + d.xy, 0.0).rgb;
    c += textureLod(tex, uv + d.zy, 0.0).rgb;
    c += textureLod(tex, uv + d.xw, 0.0).rgb;
    c += textureLod(tex, uv + d.zw, 0.0).rgb;

    return c * 0.25;
}

void main()
{
    vec2 uv = (2.0 * gl_FragCoord.xy) / vec2(textureSize(RT, 0));
    vec2 tsize = 1.0 / vec2(textureSize(RT, 0));

    FragColor.rgb = DownsampleBox4Tap(RT, uv, tsize);
}
