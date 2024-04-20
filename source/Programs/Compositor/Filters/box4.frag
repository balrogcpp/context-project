// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L43
vec3 Box4(sampler2D tex, const vec2 uv)
{
    vec2 tsize = 1.0 / vec2(textureSize(tex, 0));

    vec3 A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).rgb;
    vec3 B = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).rgb;
    vec3 C = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).rgb;
    vec3 D = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).rgb;

    vec3 c1 = (A + B + C + D) * 0.25;

    return c1;
}

void main()
{
    FragColor.rgb = Box4(RT, gl_FragCoord.xy / vec2(textureSize(RT, 0)));
}
