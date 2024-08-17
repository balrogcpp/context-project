// created by Andrey Vasiliev

#include "header.glsl"

#ifdef SHADOWCASTER_ALPHA
uniform sampler2D AlbedoTex;
#endif

#ifdef SHADOWCASTER_ALPHA
in highp vec2 vUV0;
#endif
void main()
{
#ifdef SHADOWCASTER_ALPHA
    if (texture(AlbedoTex, vUV0.xy).a < 0.5) discard;
#endif
}
