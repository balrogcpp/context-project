// created by Andrey Vasiliev

#ifndef HOSEK_GLSL
#define HOSEK_GLSL

// Ground albedo and turbidy are baked into the lookup tables
#define ALBEDO 0
#define TURBIDITY 4
#define M_PI 3.1415926535897932384626433832795
#define CIE_X 0
#define CIE_Y 1
#define CIE_Z 2

#if ALBEDO > 0
const mediump float kHosekCoeffsX[] = float[](
-1.171419, -0.242975, -8.991334, 9.571216, -0.027729, 0.668826, 0.076835,3.785611,0.634764, -1.228554, -0.291756, 2.753986, -2.491780, -0.046634, 0.311830, 0.075465, 4.463096, 0.595507, -1.093124, -0.244777, 0.909741, 0.544830, -0.295782, 2.024167, -0.000515, -1.069081, 0.936956, -1.056994, 0.015695, -0.821749, 1.870818, 0.706193, -1.483928, 0.597821, 6.864902, 0.367333, -1.054871, -0.275813, 2.712807, -5.950110, -6.554039, 2.447523, -0.189517, -1.454292, 0.913174, -1.100218, -0.174624, 1.438505, 11.154810, -3.266076, -0.883736, 0.197010, 1.991595, 0.590782
);

const mediump float kHosekCoeffsY[] = float[](
-1.185983, -0.258118, -7.761056, 8.317053, -0.033518, 0.667667, 0.059417, 3.820727, 0.632403, -1.268591, -0.339807, 2.348503, -2.023779, -0.053685, 0.108328, 0.084029, 3.910254, 0.557748, -1.071353, -0.199246, 0.787839, 0.197470, -0.303306, 2.335298, -0.082053, 0.795445, 0.997231, -1.089513, -0.031044, -0.599575, 2.330281, 0.658194, -1.821467, 0.667997, 5.090195, 0.312516, -1.040214, -0.257093, 2.660489, -6.506045, -7.053586, 2.763153, -0.243363, -0.764818, 0.945294, -1.116052, -0.183199, 1.457694, 11.636080, -3.216426, -1.045594, 0.228500, 1.817407, 0.581040
);

const mediump float kHosekCoeffsZ[] = float[](
-1.354183, -0.513062, -42.192680, 42.717720, -0.005365, 0.413674, 0.012352, 2.520122, 0.518727, -1.741434, -0.958976, -8.230339, 9.296799, -0.009600, 0.499497, 0.029555, 0.366710, 0.352700, -0.691735, 0.215489, -0.876026, 0.233412, -0.019096, 0.474803, -0.113851, 6.515360, 1.225097, -1.293189, -0.421870, 1.620952, -0.785860, -0.037694, 0.663679, 0.336494, -0.534102, 0.212835, -0.973552, -0.132549, 1.007517, 0.259826, 0.067622, 0.001421, -0.069160, 3.185897, 0.864196, -1.094800, -0.196206, 0.575559, 0.290626, 0.262575, 0.764405, 0.134749, 2.677126, 0.646546
);

const mediump float kHosekRadX[] = float[](
1.468395, 2.211970, -2.845869, 20.750270, 15.248220, 19.376220
);

const mediump float kHosekRadY[] = float[](
1.516536, 2.438729, -3.624121, 22.986210, 15.997820, 20.700270
);

const mediump float kHosekRadZ[] = float[](
1.234428, 2.289628, -3.404699, 14.994360, 34.683900, 30.848420
);
#else
const mediump float kHosekCoeffsX[] = float[](
-1.129483, -1.14642, -1.128348, -1.02582, -1.083957, -1.062465, -0.189062, -0.188358, -0.26413, 0.014047, -0.260668, -0.151219, -9.065101, 3.309173, 1.223176, -1.187406, 2.207108, 1.042881, 9.659923, -3.127882, 0.05515, 2.7299, -7.202803, 14.27839, -0.036078, -0.069382, -0.349065, 0.587759, -5.968103, -4.242214, 0.831436, 0.398711, 1.997784, -0.276114, 2.129455, 0.40381, 0.081817, 0.140058, -0.041237, 0.460263, -0.077895, 0.199778, 4.768868, 6.283042, -2.251251, 8.305125, -1.137688, 2.814449, 0.633978, 0.526708, 0.948347, 0.3945, 0.887177, 0.58032, 0.58032
);

const mediump float kHosekCoeffsY[] = float[](
-1.144464, -1.170104, -1.129171, -1.042294, -1.082293, -1.071715, -0.20438, -0.211863, -0.255288, 0.00445, -0.272306, -0.142657, -10.20188, 4.391405, 0.22383, -0.511603, 2.065076, 1.095351, 10.71247, -4.1989, 0.731429, 2.627589, -8.143133, 17.29783, -0.032567, -0.071116, -0.356273, 0.6099, -7.892212, -3.851931, 0.786021, 0.389044, 1.881931, -0.126464, 2.142231, 0.436051, 0.068727, 0.102483, -0.030787, 0.432528, -0.071062, 0.211444, 4.824771, 6.282535, -1.03912, 7.080503, -1.122398, 2.970832, 0.625984, 0.536569, 0.90963, 0.458365, 0.833851, 0.594439, 0.594439
);

const mediump float kHosekCoeffsZ[] = float[](
-1.353023, -1.624704, -0.798361, -1.266679, -1.009707, -1.075646, -0.481352, -0.79902, 0.141748, -0.428898, -0.153775, -0.176875, -31.0492, -21.67125, 9.914841, -5.818701, 3.496378, -1.347762, 31.40156, 22.46341, -10.81503, 6.986437, -3.013726, 1.989004, -0.009511, -0.011635, -0.012188, -0.081807, 0.242115, 0.013758, 0.554203, 0.541575, 0.341139, 1.397403, -0.283193, 1.76481, 0.008135, 0.026184, -0.061377, 0.201692, 0.030034, 0.133002, 3.136646, 1.139214, 7.445848, -1.275731, 3.702862, 3.230864, 0.521599, 0.344436, 1.18008, 0.259277, 0.774632, 0.662621, 0.662621
);

const mediump float kHosekRadX[] = float[](
1.471043, 1.746088, -0.92997, 17.20362, 5.473384, 8.336416
);

const mediump float kHosekRadY[] = float[](
1.522034, 1.844545, -1.322862, 19.18382, 5.440769, 8.837119
);

const mediump float kHosekRadZ[] = float[](
1.107408, 2.382765, -5.112357, 21.47823, 14.93128, 14.60882
);
#endif

mediump float sample_coeff(const int channel, const int quintic_coeff, const int coeff)
{
#if ALBEDO > 0
    int index = 9 * quintic_coeff + coeff;
#else
    int index = 6 * coeff + quintic_coeff;
#endif
    if (channel == CIE_X) return kHosekCoeffsX[index];
    else if (channel == CIE_Y) return kHosekCoeffsY[index];
    else if (channel == CIE_Z) return kHosekCoeffsZ[index];
}

mediump float sample_radiance(const int channel, const int quintic_coeff)
{
    int index = quintic_coeff;
    if (channel == CIE_X) return kHosekRadX[index];
    else if (channel == CIE_Y) return kHosekRadY[index];
    else if (channel == CIE_Z) return kHosekRadZ[index];
}

mediump float eval_quintic_bezier(const mediump float[6] control_points, const mediump float t)
{
    mediump float t2 = t * t;
    mediump float t3 = t2 * t;
    mediump float t4 = t3 * t;
    mediump float t5 = t4 * t;

    mediump float t_inv = 1.0 - t;
    mediump float t_inv2 = t_inv * t_inv;
    mediump float t_inv3 = t_inv2 * t_inv;
    mediump float t_inv4 = t_inv3 * t_inv;
    mediump float t_inv5 = t_inv4 * t_inv;

    return (
    control_points[0] *             t_inv5 +
    control_points[1] *  5.0 * t  * t_inv4 +
    control_points[2] * 10.0 * t2 * t_inv3 +
    control_points[3] * 10.0 * t3 * t_inv2 +
    control_points[4] *  5.0 * t4 * t_inv  +
    control_points[5] *        t5
    );
}

mediump float transform_sun_zenith(const mediump float sun_zenith)
{
    mediump float elevation = M_PI / 2.0 - sun_zenith;
    return pow(elevation / (M_PI / 2.0), 0.333333);
}

void get_control_points(const int channel, const int coeff, out mediump float[6] control_points)
{
    for (int i = 0; i < 6; ++i) control_points[i] = sample_coeff(channel, i, coeff);
}

void get_control_points_radiance(const int channel, out mediump float[6] control_points)
{
    for (int i = 0; i < 6; ++i) control_points[i] = sample_radiance(channel, i);
}

void get_coeffs(const int channel, const mediump float sun_zenith, out float[9] coeffs)
{
    mediump float t = transform_sun_zenith(sun_zenith);
    for (int i = 0; i < 9; ++i) {
        mediump float control_points[6];
        get_control_points(channel, i, control_points);
        coeffs[i] = eval_quintic_bezier(control_points, t);
    }
}

mediump vec3 mean_spectral_radiance(const mediump float sun_zenith)
{
    mediump vec3 spectral_radiance;
    for (int i = 0; i < 3; ++i) {
        mediump float control_points[6];
        get_control_points_radiance(i, control_points);
        mediump float t = transform_sun_zenith(sun_zenith);
        spectral_radiance[i] = eval_quintic_bezier(control_points, t);
    }
    return spectral_radiance;
}

float HosekWilkie(const highp float cos_theta, const highp float gamma, const in highp float[9] coeffs)
{
    highp float A = coeffs[0];
    highp float B = coeffs[1];
    highp float C = coeffs[2];
    highp float D = coeffs[3];
    highp float E = coeffs[4];
    highp float F = coeffs[5];
    highp float G = coeffs[6];
    highp float H = coeffs[8];
    highp float I = coeffs[7];
    highp float cos_gamma = cos(gamma);
    highp float chi = (1.0 + pow(cos_gamma, 2.0)) / pow(1.0 + H*H - 2.0 * H * cos_gamma, 1.5);

    return (
    (1.0 + A * exp(B / (cos_theta + 0.01))) *
    (C + D * exp(E * gamma) + F * pow(cos_gamma, 2.0) + G * chi + I * sqrt(cos_theta))
    );
}

highp vec3 spectral_radiance(const highp float cos_theta, const highp float gamma, const highp float sun_zenith)
{
    highp vec3 XYZ;
    for (int i = 0; i < 3; ++i) {
        highp float coeffs[9];
        get_coeffs(i, sun_zenith, coeffs);
        XYZ[i] = HosekWilkie(cos_theta, gamma, coeffs);
    }
    return XYZ;
}

// Returns angle between two directions defined by zentih and azimuth angles
mediump float angle(const mediump float z1, const mediump float a1, const mediump float z2, const mediump float a2) {
    return acos(
    sin(z1) * cos(a1) * sin(z2) * cos(a2) +
    sin(z1) * sin(a1) * sin(z2) * sin(a2) +
    cos(z1) * cos(z2));
}

highp vec3 sample_sky(const highp float cos_theta, const highp float gamma, const highp float sun_zenith, const highp float sun_azimuth)
{
    return XYZtoRGB(spectral_radiance(cos_theta, gamma, sun_zenith) * mean_spectral_radiance(sun_zenith));
}

#endif // HOSEK_GLSL
