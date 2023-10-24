// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D Lum;
uniform vec2 BrightThreshold;

// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L15
vec3 Downscale13(sampler2D tex, vec2 uv, vec2 tsize)
{
    vec3 A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).xyz;
    vec3 B = texture2D(tex, uv + tsize * vec2( 0.0, -1.0)).xyz;
    vec3 C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).xyz;
    vec3 D = texture2D(tex, uv + tsize * vec2(-0.5, -0.5)).xyz;
    vec3 E = texture2D(tex, uv + tsize * vec2( 0.5, -0.5)).xyz;
    vec3 F = texture2D(tex, uv + tsize * vec2(-1.0,  0.0)).xyz;
    vec3 G = texture2D(tex, uv                           ).xyz;
    vec3 H = texture2D(tex, uv + tsize * vec2( 1.0,  0.0)).xyz;
    vec3 I = texture2D(tex, uv + tsize * vec2(-0.5,  0.5)).xyz;
    vec3 J = texture2D(tex, uv + tsize * vec2( 0.5,  0.5)).xyz;
    vec3 K = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).xyz;
    vec3 L = texture2D(tex, uv + tsize * vec2( 0.0,  1.0)).xyz;
    vec3 M = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).xyz;

    vec3 c1 = (D + E + I + J) * 0.125;
    vec3 c2 = (A + B + G + F) * 0.03125;
    vec3 c3 = (B + C + H + G) * 0.03125;
    vec3 c4 = (F + G + L + K) * 0.03125;
    vec3 c5 = (G + H + M + L) * 0.03125;

    return c1 + c2 + c3 + c4 + c5;
}

in vec2 vUV0;
void main()
{
    vec2 TexelSize0 = 1.0 / vec2(textureSize(RT, 0));
    vec3 color = Downscale13(RT, vUV0, TexelSize0);
    float lum = texture2D(Lum, vec2(0.0, 0.0)).r;

    color *= lum;
	vec3 w = clamp((color - BrightThreshold.xxx) * BrightThreshold.yyy, 0.0, 1.0);
	color *= w * w * (3.0 - 2.0 * w);
    FragColor.rgb = color;
}
