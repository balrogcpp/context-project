// created by Andrey Vasiliev
//? #version 400

uniform highp sampler2D RT;
uniform highp vec4 ZBufferParams;

// https://github.com/OGRECave/ogre-next/blob/359a28aaaeb37812978775c7976c5028f9c7fb4b/Samples/Media/2.0/scripts/materials/Common/GLSL/DepthDownscaleMax_ps.glsl
out float FragColor;
void main()
{
#if __VERSION__ > 330
    highp vec4 c = textureGather(RT, 2.0 * gl_FragCoord.xy / vec2(textureSize(RT, 0)), 0);
    highp float z = max(max(c.x, c.y), max(c.z, c.w));
#else
    ivec2 uv = ivec2(gl_FragCoord.xy * 2.0);
    highp float fDepth0 = texelFetch(RT, uv, 0).x;
    highp float fDepth1 = texelFetchOffset(RT, uv, 0, ivec2(0, 1)).x;
    highp float fDepth2 = texelFetchOffset(RT, uv, 0, ivec2(1, 0)).x;
    highp float fDepth3 = texelFetchOffset(RT, uv, 0, ivec2(1, 1)).x;
    highp float z = max(max(fDepth0, fDepth1), max(fDepth2, fDepth3));
#endif

    FragColor = 1.0 / (z * ZBufferParams.x + ZBufferParams.y);
}
