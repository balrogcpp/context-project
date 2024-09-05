// created by Andrey Vasiliev

#include "header.glsl"
#include "SMAA_GLSL.glsl"

#define SMAA_INCLUDE_VS 0
#define SMAA_INCLUDE_PS 1
#define SMAA_REPROJECTION 0

//Can be sRGB
uniform sampler2D rt_input;
uniform sampler2D blendTex;
#if SMAA_REPROJECTION
uniform sampler2D velocityTex;
#endif
uniform vec4 ViewportSize;

#include "smaa.glsl"

in vec4 offset;
void main()
{
	vec2 size = vec2(textureSize(rt_input, 0));
    vec2 tsize = 1.0 / size;
    vec2 uv = gl_FragCoord.xy / size;
    uv.y = 1.0 - uv.y;

#if SMAA_REPROJECTION
	FragColor.rgb = SMAANeighborhoodBlendingPS(uv, offset, rt_input, blendTex, velocityTex).rgb;
#else
	FragColor.rgb = SMAANeighborhoodBlendingPS(uv, offset, rt_input, blendTex).rgb;
#endif
}
