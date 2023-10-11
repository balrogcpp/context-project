// created by Andrey Vasiliev

#ifdef OGRE_FRAGMENT_SHADER
#ifdef CHECKERBOARD
uniform mediump float IsEven;

bool ExcludePixel()
{
    return mod(floor(gl_FragCoord.y) + floor(gl_FragCoord.x), 2.0) == IsEven;
}
#else
bool ExcludePixel()
{
    return false;
}
#endif
#endif // OGRE_FRAGMENT_SHADER
