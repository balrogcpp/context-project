// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform highp vec4 ZBufferParams;

// https://github.com/OGRECave/ogre-next/blob/359a28aaaeb37812978775c7976c5028f9c7fb4b/Samples/Media/2.0/scripts/materials/Common/GLSL/DepthDownscaleMax_ps.glsl
void main()
{
    highp float z = texelFetch(RT, ivec2(gl_FragCoord.xy), 0).x;

    FragColor.x = 1.0 / (z * ZBufferParams.x + ZBufferParams.y);
}
