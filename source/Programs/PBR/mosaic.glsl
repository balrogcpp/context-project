// created by Andrey Vasiliev

#ifdef OGRE_FRAGMENT_SHADER
#ifdef CHECKERBOARD
uniform mediump float IsEven;

bool ExcludePixel()
{
    return mod(floor(gl_FragCoord.y) + floor(gl_FragCoord.x), 2.0) == IsEven;
}

bool PixelWasRenderedThisFrame(mediump vec2 uv, mediump vec2 tex_size)
{
    return mod(floor(uv.y * tex_size.y) + floor(uv.x * tex_size.x), 2.0) != IsEven;
}

bool PixelWasRenderedPrevFrame(mediump vec2 uv, mediump vec2 tex_size)
{
    return mod(floor(uv.y * tex_size.y) + floor(uv.x * tex_size.x), 2.0) == IsEven;
}

bool PixelIsInsideViewport(mediump vec2 uv)
{
    return uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0;
}

#else
bool ExcludePixel()
{
    return false;
}
#endif
#endif // OGRE_FRAGMENT_SHADER
