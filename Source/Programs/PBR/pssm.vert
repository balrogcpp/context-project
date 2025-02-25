// created by Andrey Vasiliev
//? #version 400

uniform highp mat4 WorldViewProjMatrix;

in highp vec4 vertex;
#if defined(SHADOWCASTER_ALPHA)
in vec2 uv0;
out highp vec2 vUV0;
#endif
void main()
{
#ifdef SHADOWCASTER_ALPHA
    vUV0.xy = uv0.xy;
#endif

    gl_Position = WorldViewProjMatrix * vertex;
}
