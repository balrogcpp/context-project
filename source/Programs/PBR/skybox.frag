// created by Andrey Vasiliev

#define HAS_MRT
#include "header.glsl"
#include "tonemap.glsl"
#include "math.glsl"
#include "srgb.glsl"

uniform vec3 LightColor0;
uniform vec3 LightDir0;
uniform float Time;


const float rayleigh  = 2.0;
const vec4 rayleigh_color = vec4(0.26, 0.41, 0.58, 1.0);
const float mie  = 0.005;
const float mie_eccentricity  = 0.8;
const vec4 mie_color = vec4(0.63, 0.77, 0.92, 1.0);

const float turbidity  = 10.0;
const float sun_disk_scale  = 1.0;
const vec4 ground_color  = vec4(1.0, 1.0, 1.0 ,1.0);
const float exposure = 0.1;

const vec3 UP = vec3(0.0, 1.0, 0.0);

// Sun constants
const float SOL_SIZE = 0.00872663806;
const float SUN_ENERGY = 1000.0;

// optical length at zenith for molecules
const float rayleigh_zenith_size = 8.4e3;
const float mie_zenith_size = 1.25e3;

const float LIGHT0_ENERGY = 1.0;

// Phase function
float henyey_greenstein(float cos_theta, float g) {
    const float k = 0.0795774715459;
    return k * (1.0 - g * g) / (pow(1.0 + g * g - 2.0 * g * cos_theta, 1.5));
}

// Simple Analytic sky. In a real project you should use a texture
vec3 atmosphere(const vec3 eye_dir) {
    float zenith_angle = clamp( dot(UP, normalize(-LightDir0)), -1.0, 1.0 );
    float sun_energy = max(0.0, 1.0 - exp(-((PI * 0.5) - acos(zenith_angle)))) * SUN_ENERGY * LIGHT0_ENERGY;
    float sun_fade = 1.0 - clamp(1.0 - exp(-LightDir0.y), 0.0, 1.0);

    // Rayleigh coefficients.
    float rayleigh_coefficient = rayleigh - ( 1.0 * ( 1.0 - sun_fade ) );
    vec3 rayleigh_beta = rayleigh_coefficient * rayleigh_color.rgb * 0.0001;
    // mie coefficients from Preetham
    vec3 mie_beta = turbidity * mie * mie_color.rgb * 0.000434;

    // optical length
    float zenith = acos(max(0.0, dot(UP, eye_dir)));
    float optical_mass = 1.0 / (cos(zenith) + 0.15 * pow(93.885 - degrees(zenith), -1.253));
    float rayleigh_scatter = rayleigh_zenith_size * optical_mass;
    float mie_scatter = mie_zenith_size * optical_mass;

    // light extinction based on thickness of atmosphere
    vec3 extinction = exp(-(rayleigh_beta * rayleigh_scatter + mie_beta * mie_scatter));

    // in scattering
    float cos_theta = dot(eye_dir, normalize(-LightDir0));

    float rayleigh_phase = (3.0 / (16.0 * PI)) * (1.0 + pow(cos_theta * 0.5 + 0.5, 2.0));
    vec3 betaRTheta = rayleigh_beta * rayleigh_phase;

    float mie_phase = henyey_greenstein(cos_theta, mie_eccentricity);
    vec3 betaMTheta = mie_beta * mie_phase;

    vec3 Lin = pow(sun_energy * ((betaRTheta + betaMTheta) / (rayleigh_beta + mie_beta)) * (1.0 - extinction), vec3(1.5));
    // Hack from https://github.com/mrdoob/three.js/blob/master/examples/jsm/objects/Sky.js
    Lin *= mix(vec3(1.0), pow(sun_energy * ((betaRTheta + betaMTheta) / (rayleigh_beta + mie_beta)) * extinction, vec3(0.5)), clamp(pow(1.0 - zenith_angle, 5.0), 0.0, 1.0));

    // Hack in the ground color
    Lin  *= mix(ground_color.rgb, vec3(1.0), smoothstep(-0.1, 0.1, dot(UP, eye_dir)));

    // Solar disk and out-scattering
    float sunAngularDiameterCos = cos(SOL_SIZE * sun_disk_scale);
    float sunAngularDiameterCos2 = cos(SOL_SIZE * sun_disk_scale*0.5);
    float sundisk = smoothstep(sunAngularDiameterCos, sunAngularDiameterCos2, cos_theta);
    vec3 L0 = (sun_energy * 1900.0 * extinction) * sundisk * LightColor0;
    // Note: Add nightime here: L0 += night_sky * extinction

    vec3 color = (Lin + L0) * 0.04;
    color = pow(color, vec3(1.0 / (1.2 + (1.2 * sun_fade))));
    color *= exposure;
    return color;
}

in highp vec3 vUV0;
void main()
{
    vec3 color = atmosphere(normalize(vUV0));

    FragColor = vec4(unreal(((color))), 1.0);
}
