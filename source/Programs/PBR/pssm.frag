//// created by Andrey Vasiliev

#include "header.glsl"

#ifdef SHADOWCASTER_ALPHA
uniform sampler2D AlbedoTex;
#endif

#ifdef SHADOWCASTER_ALPHA
in vec2 vUV0;
#endif
void main()
{
#ifdef SHADOWCASTER_ALPHA
    if (texture2D(AlbedoTex, vUV0.xy).a < 0.5) discard;
#endif

    FragColor.r = (gl_FragCoord.z - 1.0) / 64.0;
}
