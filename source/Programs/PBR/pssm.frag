// created by Andrey Vasiliev

#include "header.glsl"

#ifdef SHADOWCASTER_ALPHA
uniform sampler2D AlbedoTex;
#endif

#ifdef SHADOWCASTER_ALPHA
in highp vec2 vUV0;
#endif
out float FragColor;
void main()
{
#ifdef SHADOWCASTER_ALPHA
    if (textureLod(AlbedoTex, vUV0.xy, 0.0).a < 0.5) discard;
#endif

    FragColor = (gl_FragCoord.z - PSSM_GLOBAL_MIN_DEPTH) / (PSSM_GLOBAL_RANGE - PSSM_GLOBAL_MIN_DEPTH);
}
