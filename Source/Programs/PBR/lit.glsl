#ifdef GL_ES
// min roughness such that (MIN_PERCEPTUAL_ROUGHNESS^4) > 0 in fp16 (i.e. 2^(-14/4), rounded up)
#define MIN_PERCEPTUAL_ROUGHNESS 0.089
#else
#define MIN_PERCEPTUAL_ROUGHNESS 0.045
#endif

#define MEDIUMP_FLT_MAX    65504.0
#define saturateMediump(x) min(x, MEDIUMP_FLT_MAX)

#define MIN_N_DOT_V 1e-4

// Chan 2018, "Material Advances in Call of Duty: WWII"
float computeMicroShadowing(const float NoL, float visibility) {
    float aperture = inversesqrt(1.0 - visibility);
    float microShadow = saturate(NoL * aperture);
    return microShadow * microShadow;
}

float clampNoV(const float NoV) {
    // Neubelt and Pettineo 2013, "Crafting a Next-gen Material Pipeline for The Order: 1886"
    return max(NoV, MIN_N_DOT_V);
}

// Computes x^5 using only multiply operations.
float pow5(const float x) {
    float x2 = x * x;
    return x2 * x2 * x;
}

float sq(const float x) {
    return x * x;
}

float max3(const vec3 v) {
    return max(v.x, max(v.y, v.z));
}

vec3 computeDiffuseColor(const vec3 baseColor, float metallic) {
    return baseColor.rgb * (1.0 - metallic);
}

float computeDielectricF0(const float reflectance) {
    return 0.16 * reflectance * reflectance;
}

vec3 computeF0(const vec3 baseColor, float metallic, float reflectance) {
    return baseColor.rgb * metallic + (reflectance * (1.0 - metallic));
}

float computeMetallicFromSpecularColor(const vec3 specularColor) {
    return max3(specularColor);
}

float computeRoughnessFromGlossiness(const float glossiness) {
    return 1.0 - glossiness;
}

float computeRoughnessFromSpec(const float specularity) {
    return 1.0 - specularity/128.0;
}

float perceptualRoughnessToRoughness(const float perceptualRoughness) {
    return perceptualRoughness * perceptualRoughness;
}

float roughnessToPerceptualRoughness(const float roughness) {
    return sqrt(roughness);
}

// https://google.github.io/filament/Filament.md.html#materialsystem/specularbrdf/geometricshadowing(specularg)
float V_SmithGGXCorrelated(const float roughness, const float NoV, const float NoL) {
    // Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"
    float a2 = roughness * roughness;
    // TODO: lambdaV can be pre-computed for all the lights, it should be moved out of this function
    float lambdaV = NoL * sqrt((NoV - a2 * NoV) * NoV + a2);
    float lambdaL = NoV * sqrt((NoL - a2 * NoL) * NoL + a2);
    float v = 0.5 / (lambdaV + lambdaL);
    // a2=0 => v = 1 / 4*NoL*NoV   => min=1/4, max=+inf
    // a2=1 => v = 1 / 2*(NoL+NoV) => min=1/4, max=+inf
    // clamp to the maximum value representable in mediump
    return saturateMediump(v);
}

float V_SmithGGXCorrelated_Fast(const float roughness, const float NoV, const float NoL) {
    // Hammon 2017, "PBR Diffuse Lighting for GGX+Smith Microsurfaces"
    float v = 0.5 / mix(2.0 * NoL * NoV, NoL + NoV, roughness);
    return saturateMediump(v);
}

float V_SmithGGXCorrelated_Anisotropic(const float at, const float ab, const float ToV, const float BoV,
        const float ToL, const float BoL, const float NoV, const float NoL) {
    // Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"
    // TODO: lambdaV can be pre-computed for all the lights, it should be moved out of this function
    float lambdaV = NoL * length(vec3(at * ToV, ab * BoV, NoV));
    float lambdaL = NoV * length(vec3(at * ToL, ab * BoL, NoL));
    float v = 0.5 / (lambdaV + lambdaL);
    return saturateMediump(v);
}

float V_Kelemen(const float LoH) {
    // Kelemen 2001, "A Microfacet Based Coupled Specular-Matte BRDF Model with Importance Sampling"
    return saturateMediump(0.25 / (LoH * LoH));
}

float V_Neubelt(const float NoV, const float NoL) {
    // Neubelt and Pettineo 2013, "Crafting a Next-gen Material Pipeline for The Order: 1886"
    return saturateMediump(1.0 / (4.0 * (NoL + NoV - NoL * NoV)));
}

// https://google.github.io/filament/Filament.md.html#materialsystem/specularbrdf/normaldistributionfunction(speculard)
float D_GGX(const float roughness, const float NoH, const vec3 h) {
    // Walter et al. 2007, "Microfacet Models for Refraction through Rough Surfaces"

    // In mediump, there are two problems computing 1.0 - NoH^2
    // 1) 1.0 - NoH^2 suffers floating point cancellation when NoH^2 is close to 1 (highlights)
    // 2) NoH doesn't have enough precision around 1.0
    // Both problem can be fixed by computing 1-NoH^2 in highp and providing NoH in highp as well

    // However, we can do better using Lagrange's identity:
    //      ||a x b||^2 = ||a||^2 ||b||^2 - (a . b)^2
    // since N and H are unit vectors: ||N x H||^2 = 1.0 - NoH^2
    // This computes 1.0 - NoH^2 directly (which is close to zero in the highlights and has
    // enough precision).
    // Overall this yields better performance, keeping all computations in mediump
#ifdef GL_ES
    vec3 NxH = cross(N, h);
    float oneMinusNoHSquared = dot(NxH, NxH);
#else
    float oneMinusNoHSquared = 1.0 - NoH * NoH;
#endif

    float a = NoH * roughness;
    float k = roughness / (oneMinusNoHSquared + a * a);
    float d = k * k * (1.0 / PI);
    return saturateMediump(d);
}

float D_Charlie(const float roughness, const float NoH) {
    // Estevez and Kulla 2017, "Production Friendly Microfacet Sheen BRDF"
    float invAlpha  = 1.0 / roughness;
    float cos2h = NoH * NoH;
    float sin2h = max(1.0 - cos2h, 0.0078125); // 2^(-14/2), so sin2h^2 > 0 in fp16
    return (2.0 + invAlpha) * pow(sin2h, invAlpha * 0.5) / (2.0 * PI);
}

vec3 F_Schlick(const vec3 f0, const float f90, const float VoH) {
    // Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"
    return f0 + (f90 - f0) * pow5(1.0 - VoH);
}

vec3 F_Schlick(const vec3 f0, const float VoH) {
    float f = pow5(1.0 - VoH);
    return f + f0 * (1.0 - f);
}

float F_Schlick(const float f0, const float f90, const float VoH) {
    return f0 + (f90 - f0) * pow5(1.0 - VoH);
}

float Fd_Lambert() {
    return 1.0 / PI;
}

float Fd_Burley(const float roughness, const float NoV, const float NoL, const float LoH) {
    // Burley 2012, "Physically-Based Shading at Disney"
    float f90 = 0.5 + 2.0 * roughness * LoH * LoH;
    float lightScatter = F_Schlick(1.0, f90, NoL);
    float viewScatter  = F_Schlick(1.0, f90, NoV);
    return lightScatter * viewScatter * (1.0 / PI);
}

// Energy conserving wrap diffuse term, does *not* include the divide by pi
float Fd_Wrap(const float NoL, const float w) {
    return saturate((NoL + w) / sq(1.0 + w));
}

//------------------------------------------------------------------------------
// Specular BRDF dispatch
//------------------------------------------------------------------------------

float distribution(const float roughness, const float NoH, const vec3 h) {
    return D_GGX(roughness, NoH, h);
}

float visibility(const float roughness, const float NoV, const float NoL) {
#ifndef GL_ES
    return V_SmithGGXCorrelated(roughness, NoV, NoL);
#else
    return V_SmithGGXCorrelated_Fast(roughness, NoV, NoL);
#endif
}

vec3 fresnel(const vec3 f0, const float LoH) {
#ifdef GL_ES
    return F_Schlick(f0, LoH); // f90 = 1.0
#else
    float f90 = saturate(dot(f0, vec3(50.0 * 0.33, 50.0 * 0.33, 50.0 * 0.33)));
    return F_Schlick(f0, f90, LoH);
#endif
}

/**
 * Evaluates lit materials with the standard shading model. This model comprises
 * of 2 BRDFs: an optional clear coat BRDF, and a regular surface BRDF.
 *
 * Surface BRDF
 * The surface BRDF uses a diffuse lobe and a specular lobe to render both
 * dielectrics and conductors. The specular lobe is based on the Cook-Torrance
 * micro-facet model (see brdf.fs for more details). In addition, the specular
 * can be either isotropic or anisotropic.
 *
 * Clear coat BRDF
 * The clear coat BRDF simulates a transparent, absorbing dielectric layer on
 * top of the surface. Its IOR is set to 1.5 (polyutherane) to simplify
 * our computations. This BRDF only contains a specular lobe and while based
 * on the Cook-Torrance microfacet model, it uses cheaper terms than the surface
 * BRDF's specular lobe (see brdf.fs).
 */
vec3 surfaceShading(const Light light, const PixelParams pixel, const float occlusion) {
    vec3 h = normalize(V + light.l);

    float NoV = shading_NoV;
    float NoL = saturate(light.NoL);
    float NoH = saturate(dot(N, h));
    float LoH = saturate(dot(light.l, h));

#if !defined(SHADING_MODEL_CLOTH)
    float D = distribution(pixel.roughness, NoH, h);
    float V = visibility(pixel.roughness, NoV, NoL);
    vec3  F = fresnel(pixel.f0, LoH);
#else
    float D = D_Charlie(pixel.roughness, NoH);
    float V = V_Neubelt(NoV, NoL);
    vec3  F = pixel.f0;
#endif

    vec3 Fr = (D * V) * F;
    float diffuse = Fd_Lambert();

#if defined(MATERIAL_HAS_SUBSURFACE_COLOR)
    // Energy conservative wrap diffuse to simulate subsurface scattering
    diffuse *= Fd_Wrap(dot(shading_normal, light.l), 0.5);
#endif

    vec3 Fd = pixel.diffuseColor * diffuse;

    // The energy compensation term is used to counteract the darkening effect
    // at high roughness
    vec3 color = (Fd + Fr * pixel.energyCompensation) * (NoL * occlusion);

#if defined(SHADING_MODEL_SUBSURFACE)
    // subsurface scattering
    // Use a spherical gaussian approximation of pow() for forwardScattering
    // We could include distortion by adding shading_normal * distortion to light.l
    float scatterVoH = saturate(dot(shading_view, -light.l));
    float forwardScatter = exp2(scatterVoH * pixel.subsurfacePower - pixel.subsurfacePower);
    float backScatter = saturate(NoL * pixel.thickness + (1.0 - pixel.thickness)) * 0.5;
    float subsurface = mix(backScatter, 1.0, forwardScatter) * (1.0 - pixel.thickness);
    color += pixel.subsurfaceColor * (subsurface * Fd_Lambert());
#endif

    // https://google.github.io/filament/Filament.md.html#materialsystem/improvingthebrdfs/energylossinspecularreflectance
    return (color * light.colorIntensity) * light.attenuation;
}
