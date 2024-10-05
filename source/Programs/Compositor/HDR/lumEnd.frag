// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D Lum;
uniform vec3 Exposure;
uniform float timeSinceLast;

float expose(const float color, const vec3 exposure) {
    return exposure.x / exp(clamp(color, exposure.y, exposure.z));
}

// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L43
float DownsampleBox4Tap(const sampler2D tex, const ivec2 uv)
{
#if __VERSION__ > 330
    vec4 g = textureGather(tex, uv / textureSize(tex, 0).xy, 0);
    float o = dot(g, vec4(1.0, 1.0, 1.0, 1.0));
#else
    float o = texelFetch(tex, uv, 0).r;
    o += texelFetchOffset(tex, uv, 0, ivec2(0, 1)).r;
    o += texelFetchOffset(tex, uv, 0, ivec2(1, 0)).r;
    o += texelFetchOffset(tex, uv, 0, ivec2(1, 1)).r;
#endif

    return o * 0.25;
}

out float FragColor;
void main()
{
    ivec2 uv = ivec2(gl_FragCoord.xy * 2.0);

    float newLum = DownsampleBox4Tap(RT, uv);
    newLum = expose(newLum, Exposure);
    float oldLum = texelFetch(Lum, ivec2(0, 0), 0).r;

    //Adapt luminicense based 75% per second.
    FragColor = mix(newLum, oldLum, pow(0.25, timeSinceLast));
}
