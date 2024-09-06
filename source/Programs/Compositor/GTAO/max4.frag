// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

uniform float FarClipDistance;
uniform float NearClipDistance;


// https://github.com/OGRECave/ogre-next/blob/359a28aaaeb37812978775c7976c5028f9c7fb4b/Samples/Media/2.0/scripts/materials/Common/GLSL/DepthDownscaleMax_ps.glsl
void main()
{
    ivec2 uv = ivec2(gl_FragCoord.xy * 2.0);

    float fDepth0 = texelFetch(RT, uv, 0).x;
    float fDepth1 = texelFetch(RT, uv + ivec2(0, 1), 0).x;
    float fDepth2 = texelFetch(RT, uv + ivec2(1, 0), 0).x;
    float fDepth3 = texelFetch(RT, uv + ivec2(1, 1), 0).x;

    float y = FarClipDistance / NearClipDistance;
    float x = (1.0 - y);
//    float w = 1.0 / FarClipDistance;
//    float z = x * w;
    float fDepth = max(max(fDepth0, fDepth1), max(fDepth2, fDepth3));
    FragColor.r = 1.0 / (fDepth * x + y);
//    FragColor.r = 1.0 / (fDepth * z + w);
}
