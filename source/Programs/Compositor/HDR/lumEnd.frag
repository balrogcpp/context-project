// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D Lum;
uniform vec2 TexelSize;
uniform vec3 Exposure;
uniform float timeSinceLast;

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

float expose(float color, const vec3 exposure) {
    return exposure.x / exp(clamp(color, exposure.y, exposure.z));
}

in highp vec2 vUV0;
void main()
{
    float newLum = Box4(RT, vUV0, TexelSize);
    newLum = expose(newLum, Exposure);
    float oldLum = texelFetch(Lum, ivec2(0, 0), 0).r;

    //Adapt luminicense based 75% per second.
    FragColor.r = mix(newLum, oldLum, pow(0.25, timeSinceLast));
}
