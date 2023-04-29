// created by Andrey Vasiliev

#ifndef TONEMAP_GLSL
#define TONEMAP_GLSL


// https://stackoverflow.com/questions/596216/formula-to-determine-perceived-brightness-of-rgb-color
//----------------------------------------------------------------------------------------------------------------------
// https://en.wikipedia.org/wiki/Relative_luminance
mediump float luminance(const mediump vec3 color)
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}


//----------------------------------------------------------------------------------------------------------------------
// https://www.w3.org/TR/AERT/#color-contrast
mediump float luminance2(const mediump vec3 color)
{
    return dot(color, vec3(0.299, 0.587, 0.114));
}


//----------------------------------------------------------------------------------------------------------------------
// https://alienryderflex.com/hsp.html
mediump float luminance3(const mediump vec3 color)
{
    return sqrt(dot(color * color, vec3(0.299, 0.587, 0.114)));
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 expose(const mediump vec3 color, const mediump float exposure)
{
    return vec3(1.0, 1.0 , 1.0) - exp(-color.rgb * exposure);
}


//----------------------------------------------------------------------------------------------------------------------
vec4 expose(const mediump vec4 color, const mediump float exposure)
{
    return vec4(expose(color.rgb, exposure), color.a);
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 tonemap(const mediump vec3 inColour, const mediump float lum)
{
    #define MIDDLE_GREY 0.72
    #define FUDGE 0.001
    #define L_WHITE 1.5

    // From Reinhard et al
    // "Photographic Tone Reproduction for Digital Images"

    // Initial luminence scaling (equation 2)
    mediump vec3 color = inColour * (MIDDLE_GREY / (FUDGE + lum));

    // Control white out (equation 4 nom)
    color *= (1.0 + color / L_WHITE);

    // Final mapping (equation 4 denom)
    color /= (1.0 + color);

    return color;
}


#endif // TONEMAP_GLSL
