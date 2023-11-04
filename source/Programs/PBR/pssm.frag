// created by Andrey Vasiliev

#include "header.glsl"

uniform vec4 DepthRange;

#ifdef SHADOWCASTER_ALPHA
uniform sampler2D AlbedoTex;
#endif

#ifdef SHADOWCASTER_ALPHA
in highp vec2 vUV0;
#endif
void main()
{
#ifdef SHADOWCASTER_ALPHA
    if (texture2D(AlbedoTex, vUV0.xy).a < 0.5) discard;
#endif

    highp float linearDepth = gl_FragCoord.z;
    highp float clampedDepth = (linearDepth - DepthRange.x) * DepthRange.w;
    FragColor.r = exp(PSSM_ESM_K * clampedDepth);
}
