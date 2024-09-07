// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform vec4 ZBufferParams;

// https://github.com/OGRECave/ogre-next/blob/359a28aaaeb37812978775c7976c5028f9c7fb4b/Samples/Media/2.0/scripts/materials/Common/GLSL/DepthDownscaleMax_ps.glsl
void main()
{
#ifndef GL_ES
    vec4 c = textureGather(RT, 2.0 * gl_FragCoord.xy / vec2(textureSize(RT, 0)), 0);

    float fDepth = max(max(c.x, c.y), max(c.z, c.w));
#else
    float fDepth0 = texelFetch(RT, ivec2(gl_FragCoord.xy * 2.0), 0).x;
    float fDepth1 = texelFetch(RT, ivec2(gl_FragCoord.xy * 2.0) + ivec2(0, 1), 0).x;
    float fDepth2 = texelFetch(RT, ivec2(gl_FragCoord.xy * 2.0) + ivec2(1, 0), 0).x;
    float fDepth3 = texelFetch(RT, ivec2(gl_FragCoord.xy * 2.0) + ivec2(1, 1), 0).x;

    float fDepth = max(max(fDepth0, fDepth1), max(fDepth2, fDepth3));
#endif

    FragColor.r = 1.0 / (fDepth * ZBufferParams.x + ZBufferParams.y);
}
