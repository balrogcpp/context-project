// created by Andrey Vasiliev

#define HAS_MRT
#include "header.glsl"
#include "tonemap.glsl"
#include "math.glsl"
#include "srgb.glsl"

uniform vec3 LightDir0;

float acosFast(const float x) {
    // Lagarde 2014, "Inverse trigonometric functions GPU optimization for AMD GCN architecture"
    // This is the approximation of degree 1, with a max absolute error of 9.0x10^-3
    float y = abs(x);
    float p = -0.1565827 * y + 1.570796;
    p *= sqrt(1.0 - y);
    return x >= 0.0 ? p : PI - p;
}

float acosFastPositive(const float x) {
    float p = -0.1565827 * x + 1.570796;
    return p * sqrt(1.0 - x);
}

#if 0
uniform vec3 LightColor0;

// Phase function
float henyey_greenstein(const float cos_theta, const float g) {
    const float k = 0.0795774715459;
    return k * (1.0 - g * g) / (pow(1.0 + g * g - 2.0 * g * cos_theta, 1.5));
}

float sq(const float x) {
    return x * x;
}

float pow5(const float x) {
    float x2 = x * x;
    return x2 * x2 * x;
}


// Simple Analytic sky. In a real project you should use a texture
vec3 atmosphere(const highp vec3 eye_dir, const highp vec3 light_dir) {
    const float rayleigh = 2.0;
    const vec4 rayleigh_color = vec4(0.26, 0.41, 0.58, 1.0);
    const float mie = 0.005;
    const float mie_eccentricity = 0.8;
    const vec4 mie_color = vec4(0.63, 0.77, 0.92, 1.0);

    const float turbidity = 3.0;
    const float sun_disk_scale = 1.0;
    const vec4 ground_color = vec4(1.0, 1.0, 1.0 ,1.0);
    const float exposure = 0.1;

    // Sun constants
    const float SOL_SIZE = 0.00872663806;
    const float SUN_ENERGY = 1000.0;
    const float LIGHT0_ENERGY = 1.0;

    // optical length at zenith for molecules
    const float rayleigh_zenith_size = 8.4e3;
    const float mie_zenith_size = 1.25e3;

    float zenith_angle = clamp(dot(vec3(0.0, 1.0, 0.0), light_dir), -1.0, 1.0);
    float sun_energy = max(0.0, 1.0 - exp(-((PI * 0.5) - acosFastPositive(zenith_angle)))) * SUN_ENERGY * LIGHT0_ENERGY;
    float sun_fade = 1.0 - clamp(1.0 - exp(light_dir.y), 0.0, 1.0);

    // Rayleigh coefficients.
    float rayleigh_coefficient = rayleigh - ( 1.0 * ( 1.0 - sun_fade ) );
    vec3 rayleigh_beta = rayleigh_coefficient * rayleigh_color.rgb * 0.0001;
    // mie coefficients from Preetham
    vec3 mie_beta = turbidity * mie * mie_color.rgb * 0.000434;

    // optical length
    float zenith = acosFastPositive(max(0.0, dot(vec3(0.0, 1.0, 0.0), eye_dir)));
    float optical_mass = 1.0 / (cos(zenith) + 0.15 * pow(93.885 - degrees(zenith), -1.253));
    float rayleigh_scatter = rayleigh_zenith_size * optical_mass;
    float mie_scatter = mie_zenith_size * optical_mass;

    // light extinction based on thickness of atmosphere
    vec3 extinction = exp(-(rayleigh_beta * rayleigh_scatter + mie_beta * mie_scatter));

    // in scattering
    float cos_theta = dot(eye_dir, light_dir);

    float rayleigh_phase = (3.0 / (16.0 * PI)) * (1.0 + sq(cos_theta * 0.5 + 0.5));
    vec3 betaRTheta = rayleigh_beta * rayleigh_phase;

    float mie_phase = henyey_greenstein(cos_theta, mie_eccentricity);
    vec3 betaMTheta = mie_beta * mie_phase;

    vec3 Lin = pow(sun_energy * ((betaRTheta + betaMTheta) / (rayleigh_beta + mie_beta)) * (1.0 - extinction), vec3(1.5, 1.5, 1.5));
    // Hack from https://github.com/mrdoob/three.js/blob/master/examples/jsm/objects/Sky.js
    Lin *= mix(vec3(1.0, 1.0, 1.0), sqrt(sun_energy * ((betaRTheta + betaMTheta) / (rayleigh_beta + mie_beta)) * extinction), clamp(pow5(1.0 - zenith_angle), 0.0, 1.0));

    // Hack in the ground color
    Lin  *= mix(ground_color.rgb, vec3(1.0, 1.0, 1.0), smoothstep(-0.1, 0.1, dot(vec3(0.0, 1.0, 0.0), eye_dir)));

    // Solar disk and out-scattering
    float sunAngularDiameterCos = cos(SOL_SIZE * sun_disk_scale);
    float sunAngularDiameterCos2 = cos(SOL_SIZE * sun_disk_scale*0.5);
    float sundisk = smoothstep(sunAngularDiameterCos, sunAngularDiameterCos2, cos_theta);
    vec3 L0 = (sun_energy * 1900.0 * extinction) * sundisk * LightColor0;
    // Note: Add nightime here: L0 += night_sky * extinction

    vec3 color = (Lin + L0) * 0.04;
    color = pow(color, vec3(1.0 / (1.2 + (1.2 * sun_fade))));
//    color *= 0.1;
//    color = 2.0 / (1.0 + exp(-0.1 * color)) - 1.0;
//    color = 1.0 - exp(-1.0 * color);
    return tonemap(color);
}

float getSunDisk(const float LdotV, const float sunY, const float sunPower)
{
    return pow(LdotV, mix(4.0, 8500.0, sunY) * 0.25) * sunPower;
}

float getMie(const float LdotV, const float sunY)
{
    // this is wrong for sure, but taken from original code
//    return pow(LdotV, mix(1.0, 1.0, sunY));
    return LdotV;
}

// See https://en.wikipedia.org/wiki/Rayleigh_distribution
// It's inspired, not fully based.
//
// The formula also gives us the nice property that for inputs
// where absorption is in range [0; 1] the output i also in range [0; 1]
vec3 getSkyRayleighAbsorption(const vec3 vDir, const float density)
{
    vec3 absorption = -density * vDir;
    absorption = exp2(absorption) * 2.0;
    return absorption;
}

float pow4(const float x)
{
    float x2 = x * x;
    return x2 * x2;
}

// https://github.com/OGRECave/ogre-next/blob/2dbbd284e0a03354b6382ac25888bd89f5c76b62/Samples/Media/2.0/scripts/materials/Common/Any/AtmosphereNprSky_ps.any
vec3 ogreAtmosphere(const highp vec3 eye_dir, const highp vec3 p_sunDir) {
    const float p_densityCoeff = 0.47;
    const float p_sunPower = 1.0;
    const float p_skyPower = 1.0;
    const vec3 p_cameraDisplacement = vec3(0.0, 0.0, 0.0);
    const vec3 p_skyColour = vec3(0.334, 0.57, 1.0);
    const float p_densityDiffusion = 2.0;
    const float p_borderLimit = 0.025;

    float p_sunHeight = sin(asin(p_sunDir.y) * PI);
    float p_lightDensity = p_densityCoeff / pow( max( p_sunHeight, 0.0035 ), 0.75 );
    float p_sunHeightWeight = exp2( -1.0 / p_sunHeight );
    vec3 p_sunAbsorption = vec3(getSkyRayleighAbsorption( 1.0 - p_skyColour, p_lightDensity));
    vec3 p_mieAbsorption = pow4( max( 1.0 - p_lightDensity, 0.1 )) * mix( p_skyColour, normalize(vec3(1.0,1.0,1.0)), p_sunHeightWeight );
    vec3 p_skyLightAbsorption = getSkyRayleighAbsorption( p_skyColour, p_lightDensity );
    float p_finalMultiplier = ( 0.5 + smoothstep( 0.02, 0.4, p_sunHeightWeight ) ) * p_skyPower;

    vec3 atmoCameraDir = eye_dir;

    const float LdotV = max( dot( atmoCameraDir, p_sunDir ), 0.0 );

    atmoCameraDir.y += p_densityDiffusion * 0.075 * sq( 1.0 - atmoCameraDir.y );
    atmoCameraDir += p_cameraDisplacement.xyz;
    atmoCameraDir = normalize( atmoCameraDir );

    atmoCameraDir.y = max( atmoCameraDir.y, p_borderLimit );

    // It's not a mistake. We do this twice. Doing it before p_borderLimit
    // allows the horizon to shift. Doing it after p_borderLimit lets
    // the sky to get darker as we get upwards.
    atmoCameraDir += p_cameraDisplacement.xyz;
    atmoCameraDir = normalize( atmoCameraDir );

    const float LdotV360 = dot(atmoCameraDir, p_sunDir) * 0.5 + 0.5;

    // ptDensity gets smaller as sunHeight gets bigger
    // ptDensity gets smaller as atmoCameraDir.y gets bigger
    const float ptDensity =
        p_densityCoeff / pow( max( atmoCameraDir.y / ( 1.0 - p_sunHeight ), 0.0035 ),
        mix( 0.10, p_densityDiffusion, pow( atmoCameraDir.y, 0.3 ) ) );

    const float sunDisk = getSunDisk( LdotV, p_sunHeight, p_sunPower );

    const float antiMie = max( p_sunHeightWeight, 0.08 );

    const vec3 skyAbsorption = getSkyRayleighAbsorption( p_skyColour, ptDensity );
    // const vec3 skyColourGradient = pow3( exp2( -atmoCameraDir.y / p_skyColour ),
    //										 mix( 15.5f, 1.5f, pow( p_sunHeightWeight, 0.5f ) ) );
//    const vec3 skyColourGradient = pow( exp2( -atmoCameraDir.y / p_skyColour ), vec3(1.5, 1.5, 1.5) );
    const vec3 skyColourGradient = exp2( -atmoCameraDir.y / p_skyColour * 1.5 );

    const float mie = getMie( LdotV360, p_sunHeightWeight );

    vec3 atmoColour = vec3(0.0, 0.0, 0.0);

    const vec3 sharedTerms = skyColourGradient * skyAbsorption;

    atmoColour += antiMie * sharedTerms * p_sunAbsorption;
    atmoColour += (mie * ptDensity * p_lightDensity) * sharedTerms * p_skyLightAbsorption;
    atmoColour += mie * p_mieAbsorption;
    atmoColour *= p_lightDensity;

    atmoColour *= p_finalMultiplier;
    atmoColour += sunDisk * p_skyLightAbsorption;
    return tonemap(atmoColour);
}
#endif

uniform vec3 HosekParams[10];

vec3 HosekWilkie(const highp vec3 V, const highp vec3 N)
{
    vec3 A = HosekParams[0];
    vec3 B = HosekParams[1];
    vec3 C = HosekParams[2];
    vec3 D = HosekParams[3];
    vec3 E = HosekParams[4];
    vec3 F = HosekParams[5];
    vec3 G = HosekParams[6];
    vec3 H = HosekParams[7];
    vec3 I = HosekParams[8];
    vec3 Z = HosekParams[9];

    float cos_theta = clamp(V.y, 0.0, 1.0);
    float cos_gamma = dot(V, N);
    float gamma = acosFastPositive(cos_gamma);
    float cos_gamma2 = cos_gamma * cos_gamma;
    vec3 I2 = I * I;

    vec3 chi = ((1.0 + cos_gamma2) / pow(1.0 + I2 - 2.0 * (cos_gamma * I), vec3(1.5, 1.5, 1.5)));
    vec3 color = Z * ((1.0 + A * exp(B / (cos_theta + 0.01))) * (C + D * exp(E * gamma) + F * cos_gamma2 + G * chi + H * sqrt(cos_theta)));

    // return 2.0 / (1.0 + exp(-0.1 * color)) - 1.0;
    return 1.0 - exp(-0.1 * color);
}

in highp vec3 vUV0;
void main()
{
    vec3 color = HosekWilkie(normalize(vUV0), -normalize(LightDir0));
    FragColor = vec4(color, 1.0);
}
