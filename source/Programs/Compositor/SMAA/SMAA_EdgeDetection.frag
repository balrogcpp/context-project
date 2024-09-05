// created by Andrey Vasiliev

#include "header.glsl"
#include "SMAA_GLSL.glsl"

#define SMAA_INCLUDE_VS 0
#define SMAA_INCLUDE_PS 1
#if !SMAA_INITIALIZED
	#define SMAA_EDGE_DETECTION_MODE 1
	#define SMAA_PREDICATION 0
#endif

//Must not be sRGB
uniform sampler2D rt_input;
#if SMAA_PREDICATION
uniform sampler2D depthTex;
#endif
uniform vec4 ViewportSize;

#include "smaa.glsl"

in vec4 offset[3];
void main()
{
	vec2 size = vec2(textureSize(rt_input, 0));
    vec2 tsize = 1.0 / size;
    vec2 uv = gl_FragCoord.xy / size;
    uv.y = 1.0 - uv.y;

#if !SMAA_EDGE_DETECTION_MODE || SMAA_EDGE_DETECTION_MODE == 2
	#if SMAA_PREDICATION
		FragColor.rg = SMAAColorEdgeDetectionPS(uv, offset, rt_input, depthTex);
	#else
		FragColor.rg = SMAAColorEdgeDetectionPS(uv, offset, rt_input);
	#endif
#else
	#if SMAA_PREDICATION
		FragColor.rg = SMAALumaEdgeDetectionPS(uv, offset, rt_input, depthTex);
	#else
		FragColor.rg = SMAALumaEdgeDetectionPS(uv, offset, rt_input);
	#endif
#endif
}
