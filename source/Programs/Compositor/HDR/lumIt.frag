// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform vec2 TexelSize;

// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L43
float Box4(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
    vec4 d = tsize.xyxy * vec4(-1.0, -1.0, 1.0, 1.0);

    float c = textureLod(tex, uv + d.xy, 0.0).r;
    c += textureLod(tex, uv + d.zy, 0.0).r;
    c += textureLod(tex, uv + d.xw, 0.0).r;
    c += textureLod(tex, uv + d.zw, 0.0).r;

    return c * 0.25;
}

in highp vec2 vUV0;
void main()
{
    FragColor.r = Box4(RT, vUV0, TexelSize);
}
