// created by Andrey Vasiliev
//? #version 400

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
    if (textureLod(AlbedoTex, vUV0.xy, 0.0).a < 0.5) {
        discard;
    }
#endif

    FragColor = (gl_FragCoord.z - 1.0) / 64.0;
}
