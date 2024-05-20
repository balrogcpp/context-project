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
float computeMicroShadowing(float NoL, float visibility)
{
    float aperture = inversesqrt(1.0 - visibility);
    float microShadow = saturate(NoL * aperture);
    return microShadow * microShadow;
}

float clampNoV(float NoV) {
    // Neubelt and Pettineo 2013, "Crafting a Next-gen Material Pipeline for The Order: 1886"
    return max(NoV, MIN_N_DOT_V);
}

// Computes x^5 using only multiply operations.
float pow5(float x) {
    float x2 = x * x;
    return x2 * x2 * x;
}

vec3 F_Schlick(const vec3 f0, float f90, float VoH) {
    // Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"
    return f0 + (f90 - f0) * pow5(1.0 - VoH);
}

vec3 F_Schlick(const vec3 f0, float VoH) {
    float f = pow(1.0 - VoH, 5.0);
    return f + f0 * (1.0 - f);
}

float F_Schlick(float f0, float f90, float VoH) {
    return f0 + (f90 - f0) * pow5(1.0 - VoH);
}

float Fd_Lambert() {
    return 1.0 / PI;
}

float Fd_Burley(float roughness, float NoV, float NoL, float LoH) {
    // Burley 2012, "Physically-Based Shading at Disney"
    float f90 = 0.5 + 2.0 * roughness * LoH * LoH;
    float lightScatter = F_Schlick(1.0, f90, NoL);
    float viewScatter  = F_Schlick(1.0, f90, NoV);
    return lightScatter * viewScatter * (1.0 / PI);
}

vec3 computeDiffuseColor(const vec3 baseColor, float metallic) {
    return baseColor.rgb * (1.0 - metallic);
}

float computeDielectricF0(float reflectance) {
    return 0.16 * reflectance * reflectance;
}

vec3 computeF0(const vec3 baseColor, float metallic, float reflectance) {
    return baseColor.rgb * metallic + (reflectance * (1.0 - metallic));
}

float computeRoughnessFromGlossiness(float glossiness) {
    return 1.0 - glossiness;
}

float perceptualRoughnessToRoughness(float perceptualRoughness) {
    return perceptualRoughness * perceptualRoughness;
}

float roughnessToPerceptualRoughness(float roughness) {
    return sqrt(roughness);
}

// https://google.github.io/filament/Filament.md.html#materialsystem/specularbrdf/geometricshadowing(specularg)
float V_SmithGGXCorrelated(float roughness, float NoV, float NoL) {
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

float V_SmithGGXCorrelated_Fast(float roughness, float NoV, float NoL) {
    // Hammon 2017, "PBR Diffuse Lighting for GGX+Smith Microsurfaces"
    float v = 0.5 / mix(2.0 * NoL * NoV, NoL + NoV, roughness);
    return saturateMediump(v);
}

float V_SmithGGXCorrelated_Anisotropic(float at, float ab, float ToV, float BoV,
        float ToL, float BoL, float NoV, float NoL) {
    // Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"
    // TODO: lambdaV can be pre-computed for all the lights, it should be moved out of this function
    float lambdaV = NoL * length(vec3(at * ToV, ab * BoV, NoV));
    float lambdaL = NoV * length(vec3(at * ToL, ab * BoL, NoL));
    float v = 0.5 / (lambdaV + lambdaL);
    return saturateMediump(v);
}

// https://google.github.io/filament/Filament.md.html#materialsystem/specularbrdf/normaldistributionfunction(speculard)
float D_GGX(float roughness, float NoH, const vec3 h) {
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
    float d = k * k * (1.0 / M_PI);
    return saturateMediump(d);
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
vec3 surfaceShading(const Light light, const PixelParams pixel) {
    vec3 h = normalize(V + light.l);

    float NoV = shading_NoV;
    float NoL = saturate(light.NoL);
    float VoH = saturate(dot(V, h));
    float NoH = saturate(dot(N, h));
    // float LoH = saturate(dot(light.l, h));

    float V = V_SmithGGXCorrelated(pixel.roughness, NoV, light.NoL);
    vec3 F  = F_Schlick(pixel.f0, pixel.f90, VoH); // LoH (?)
    float D = D_GGX(pixel.roughness, NoH, h);

    vec3 Fr = (D * V) * F;
    vec3 Fd = pixel.diffuseColor * Fd_Lambert();

    // https://google.github.io/filament/Filament.md.html#materialsystem/improvingthebrdfs/energylossinspecularreflectance
    return (light.colorIntensity * light.NoL) * (Fr * pixel.energyCompensation + Fd) * light.attenuation;
}
