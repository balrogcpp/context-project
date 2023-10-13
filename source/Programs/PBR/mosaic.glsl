// created by Andrey Vasiliev

#ifndef MOSAIC_GLSL
#define MOSAIC_GLSL
#ifdef OGRE_FRAGMENT_SHADER

uniform mediump float IsEven;

bool ExcludePixel()
{
    return mod(floor(gl_FragCoord.y) + floor(gl_FragCoord.x), 2.0) == IsEven;
}

bool PixelWasRenderedThisFrame(const highp vec2 uv, const highp vec2 tex_size)
{
    return mod(floor(uv.y * tex_size.y) + floor(uv.x * tex_size.x), 2.0) != IsEven;
}

bool PixelWasRenderedPrevFrame(const highp vec2 uv, const highp vec2 tex_size)
{
    return mod(floor(uv.y * tex_size.y) + floor(uv.x * tex_size.x), 2.0) == IsEven;
}

bool PixelIsInsideViewport(const highp vec2 uv)
{
    return uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0;
}

#endif // OGRE_FRAGMENT_SHADER
#endif // MOSAIC_GLSL
