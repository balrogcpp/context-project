// This source file is part of Glue Engine. Created by Andrey Vasiliev

#ifndef HDR_TONEMAP_GLSL
#define HDR_TONEMAP_GLSL

#define MIDDLE_GREY 0.72
#define FUDGE 0.001
#define L_WHITE 1.5

//----------------------------------------------------------------------------------------------------------------------
vec3 tone_map(vec3 inColour, const float lum)
{
	// From Reinhard et al
	// "Photographic Tone Reproduction for Digital Images"

	// Initial luminence scaling (equation 2)
    inColour *= MIDDLE_GREY / (FUDGE + lum);

	// Control white out (equation 4 nom)
    inColour *= (1.0 + inColour / L_WHITE);

	// Final mapping (equation 4 denom)
	inColour /= (1.0 + inColour);
	
	return inColour;
}

#endif // HDR_TONEMAP_GLSL
