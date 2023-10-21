// created by Andrey Vasiliev

#ifndef MOSAIC_GLSL
#define MOSAIC_GLSL
#ifdef OGRE_FRAGMENT_SHADER

uniform float IsEven;

bool ExcludePixel()
{
    return mod(floor(gl_FragCoord.x) + floor(gl_FragCoord.y), 2.0) == IsEven;
}

bool PixelWasRenderedThisFrame()
{
    return !ExcludePixel();
}

bool PixelWasRenderedPrevFrame()
{
    return ExcludePixel();
}

bool PixelIsInsideViewport(const vec2 uv)
{
    return uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0;
}

#endif // OGRE_FRAGMENT_SHADER
#endif // MOSAIC_GLSL
