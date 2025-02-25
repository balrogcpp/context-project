// created by Andrey Vasiliev
//? #version 400

#if defined(SHADOWCASTER_ALPHA)
uniform sampler2D AlbedoTex;
#endif

#if defined(SHADOWCASTER_ALPHA)
in highp vec2 vUV0;
#endif
void main()
{
#if defined(SHADOWCASTER_ALPHA)
    if (texture(AlbedoTex, vUV0.xy).a < 0.5) discard;
#endif
}
