// created by Andrey Vasiliev

#include "header.glsl"

uniform highp sampler2D RT;

// https://github.com/OGRECave/ogre-next/blob/359a28aaaeb37812978775c7976c5028f9c7fb4b/Samples/Media/2.0/scripts/materials/Common/GLSL/DepthDownscaleMax_ps.glsl
out highp float FragColor;
void main()
{
#if __VERSION__ > 330
    highp vec4 c = textureGather(RT, 2.0 * gl_FragCoord.xy / vec2(textureSize(RT, 0)), 0);
    FragColor = max(max(c.x, c.y), max(c.z, c.w));
#else
    // ivec2 uv = ivec2(gl_FragCoord.xy * 2.0);
    // highp float fDepth0 = texelFetch(RT, uv, 0).x;
    // highp float fDepth1 = texelFetch(RT, uv + ivec2(0, 1), 0).x;
    // highp float fDepth2 = texelFetch(RT, uv + ivec2(1, 0), 0).x;
    // highp float fDepth3 = texelFetch(RT, uv + ivec2(1, 1), 0).x;
    // FragColor = max(max(fDepth0, fDepth1), max(fDepth2, fDepth3));

    FragColor = texelFetch(RT, ivec2(gl_FragCoord.xy * 2.0), 0).x;
#endif
}
