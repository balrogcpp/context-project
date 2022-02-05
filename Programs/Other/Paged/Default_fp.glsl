#include <OgreUnifiedShader.h>

#include "srgb.glsl"

uniform SAMPLER2D(texMap, 0);

uniform vec4 fogColour;
uniform vec4 fogParams;

MAIN_PARAMETERS
IN(vec4 oUV, TEXCOORD0)
IN(vec4 oColour, COLOR)
IN(float oFogCoord, FOG)
MAIN_DECLARATION
{
    vec4 color = texture2D(texMap, oUV.xy);
#ifdef ALPHA_TEST
    if(color.a < 0.5 || oColour.a < 0.5)
        discard;
#endif
    color *= saturate(oColour);
    color = SRGBtoLINEAR(color);
    gl_FragData[0] = color;
#ifdef USE_FOG
    gl_FragData[1].r = oFogCoord / 1000.0;
#endif
}
