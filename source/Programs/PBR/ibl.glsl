// https://google.github.io/filament/Filament.html 5.4.3.1 Diffuse BRDF integration
#define MULTI_BOUNCE_AMBIENT_OCCLUSION 1
#define SPECULAR_AMBIENT_OCCLUSION 1

/**
 * Computes a specular occlusion term from the ambient occlusion term.
 */
float computeSpecularAO(float NoV, float visibility, float roughness) {
#if SPECULAR_AMBIENT_OCCLUSION == 1
    return saturate(pow(NoV + visibility, exp2(-16.0 * roughness - 1.0)) - 1.0 + visibility);
#else
    return 1.0;
#endif
}

/**
 * Returns a color ambient occlusion based on a pre-computed visibility term.
 * The albedo term is meant to be the diffuse color or f0 for the diffuse and
 * specular terms respectively.
 */
vec3 gtaoMultiBounce(float visibility, const vec3 albedo) {
    // Jimenez et al. 2016, "Practical Realtime Strategies for Accurate Indirect Occlusion"
    vec3 a =  2.0404 * albedo - 0.3324;
    vec3 b = -4.7951 * albedo + 0.6417;
    vec3 c =  2.7552 * albedo + 0.6903;

    return max(vec3(visibility, visibility, visibility), ((visibility * a + b) * visibility + c) * visibility);
}

void multiBounceAO(float visibility, const vec3 albedo, inout vec3 color) {
#if MULTI_BOUNCE_AMBIENT_OCCLUSION == 1
    color *= gtaoMultiBounce(visibility, albedo);
#endif
}

void multiBounceSpecularAO(float visibility, const vec3 albedo, inout vec3 color) {
#if MULTI_BOUNCE_AMBIENT_OCCLUSION == 1 && SPECULAR_AMBIENT_OCCLUSION == 1
    color *= gtaoMultiBounce(visibility, albedo);
#endif
}

float singleBounceAO(float visibility) {
#if MULTI_BOUNCE_AMBIENT_OCCLUSION == 1
    return 1.0;
#else
    return visibility;
#endif
}

// https://www.unrealengine.com/en-US/blog/physically-based-shading-on-mobile
vec3 EnvBRDFApprox(const vec3 specularColor, float roughness, float NoV)
{
    const vec4 c0 = vec4(-1.0, -0.0275, -0.572, 0.022);
    const vec4 c1 = vec4(1.0, 0.0425, 1.04, -0.04);
    vec4 r = roughness * c0 + c1;
    float a004 = min(r.x * r.x, exp2(-9.28 * NoV)) * r.x + r.y;
    vec2 AB = vec2(-1.04, 1.04) * a004 + r.zw;
    //return specularColor * AB.x + AB.y;
    return vec3(AB, 0.0);
}

float EnvBRDFApproxNonmetal(float roughness, float NoV)
{
    // Same as EnvBRDFApprox( 0.04, Roughness, NoV )
    const vec2 c0 = vec2(-1.0, -0.0275);
    const vec2 c1 = vec2(1.0, 0.0425);
    vec2 r = roughness * c0 + c1;
    return min(r.x * r.x, exp2(-9.28 * NoV)) * r.x + r.y;
}

vec3 specularDFG(const PixelParams pixel) {
#if defined(SHADING_MODEL_CLOTH)
    return pixel.f0 * pixel.dfg.z;
#else
    return mix(pixel.dfg.xxx, pixel.dfg.yyy, pixel.f0);
#endif
}

vec3 decodeDataForIBL(const vec3 data) {
#if defined(FORCE_TONEMAP)
    return (data);
#else
    return tonemap(data);
#endif
}

//------------------------------------------------------------------------------
// IBL prefiltered DFG term implementations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// IBL environment BRDF dispatch
//------------------------------------------------------------------------------

#ifdef HAS_LUT
vec3 PrefilteredDFG_LUT(float lod, float NoV) {
    // coord = sqrt(linear_roughness), which is the mapping used by cmgen.
    // SNiLD: Y is flipped for Ogre because there is no tool to flip the actual
    //        texture that is in 16-bit float RGBA DDS format.
    return textureLod(LUTtex, vec2(NoV, 1.0 - lod), 0.0).rgb;
}

vec3 prefilteredDFG(float perceptualRoughness, float NoV) {
    // PrefilteredDFG_LUT() takes a LOD, which is sqrt(roughness) = perceptualRoughness
    return PrefilteredDFG_LUT(perceptualRoughness, NoV);
}
#endif

#ifdef HAS_IBL
vec3 diffuseIrradiance(const vec3 n) {
    return decodeDataForIBL(textureCubeLod(SpecularEnvTex, n, 6.0).rgb);
}

float perceptualRoughnessToLod(float perceptualRoughness) {
    // The mapping below is a quadratic fit for log2(perceptualRoughness)+iblMaxMipLevel when
    // iblMaxMipLevel is 4. We found empirically that this mapping works very well for
    // a 256 cubemap with 5 levels used. But also scales well for other iblMaxMipLevel values.
    return 6.0 * perceptualRoughness * (2.0 - perceptualRoughness);
}

vec3 prefilteredRadiance(const vec3 n, float perceptualRoughness) {
    return decodeDataForIBL(textureCubeLod(SpecularEnvTex, n, perceptualRoughnessToLod(perceptualRoughness)).rgb);
}

vec3 getSpecularDominantDirection(const vec3 n, const vec3 r, float roughness) {
    return mix(r, n, roughness * roughness);
}

vec3 getReflectedVector(float roughness) {
    vec3 r = reflect(-V, N);
    r = getSpecularDominantDirection(N, r, roughness);
    r.z *= -1.0;
    return r;
}
#endif

void evaluateClothIndirectDiffuseBRDF(const PixelParams pixel, inout float diffuse) {
#if defined(SHADING_MODEL_CLOTH)
#if defined(MATERIAL_HAS_SUBSURFACE_COLOR)
    // Simulate subsurface scattering with a wrap diffuse term
    diffuse *= Fd_Wrap(shading_NoV, 0.5);
#endif
#endif
}

void evaluateSubsurfaceIBL(const PixelParams pixel, const vec3 diffuseIrradiance,
        inout vec3 Fd, inout vec3 Fr) {
#if defined(SHADING_MODEL_SUBSURFACE)
    vec3 viewIndependent = diffuseIrradiance;
    vec3 viewDependent = prefilteredRadiance(-shading_view, pixel.roughness, 1.0 + pixel.thickness);
    float attenuation = (1.0 - pixel.thickness) / (2.0 * PI);
    Fd += pixel.subsurfaceColor * (viewIndependent + viewDependent) * attenuation;
#elif defined(SHADING_MODEL_CLOTH) && defined(MATERIAL_HAS_SUBSURFACE_COLOR)
    Fd *= saturate(pixel.subsurfaceColor + shading_NoV);
#endif
}

// Calculation of the lighting contribution from an optional Image Based Light source.
// Precomputed Environment Maps are required uniform inputs and are computed as outlined in [1].
// See our README.md on Environment Maps [3] for additional discussion.
vec3 evaluateIBL(const PixelParams pixel) {
    float diffuseAO = min(pixel.occlusion, pixel.ssao);
    float specularAO = computeSpecularAO(shading_NoV, diffuseAO, pixel.roughness);

//    vec3 E = specularDFG(pixel);
    vec3 E = pixel.f0 * pixel.dfg.x + pixel.dfg.y;
    float diffuseBRDF = singleBounceAO(diffuseAO); // Fd_Lambert() is baked in the SH below

    evaluateClothIndirectDiffuseBRDF(pixel, diffuseBRDF);

#ifdef HAS_IBL
    vec3 r = getReflectedVector(pixel.roughness);

    vec3 Fr = E * prefilteredRadiance(r, pixel.perceptualRoughness);
    Fr *= singleBounceAO(specularAO) * pixel.energyCompensation;

    vec3 diffuseIrradiance = diffuseIrradiance(N);
    vec3 Fd = pixel.diffuseColor * diffuseIrradiance * (1.0 - E) * diffuseBRDF;

    // subsurface layer
    evaluateSubsurfaceIBL(pixel, diffuseIrradiance, Fd, Fr);
#else
    vec3 Fr = E;
    Fr *= singleBounceAO(specularAO) * pixel.energyCompensation;

    vec3 Fd = pixel.diffuseColor * (1.0 - E) * diffuseBRDF;
#endif

    // extra ambient occlusion term
    multiBounceAO(diffuseAO, pixel.diffuseColor, Fd);
    multiBounceSpecularAO(specularAO, pixel.f0, Fr);

    return SurfaceAmbientColour.rgb * AmbientLightColour.rgb * (Fd  + Fr);
}
