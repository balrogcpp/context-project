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

    float linearDepth = gl_FragCoord.z;
    float clampedDepth = (linearDepth - PSSM_GLOBAL_MIN_DEPTH) / PSSM_GLOBAL_RANGE;
    FragColor.r = clampedDepth;
}
