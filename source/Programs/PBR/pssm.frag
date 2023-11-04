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

    FragColor.r = (gl_FragCoord.z - DepthRange.x) * DepthRange.w;
}
