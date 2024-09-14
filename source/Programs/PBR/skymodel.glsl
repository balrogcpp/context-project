// created by Andrey Vasiliev

// Clamps color between 0 and 1 smoothly
vec3 SkyLightExpose(const vec3 color)
{
    return 1.0 - exp(-0.1 * color);
}
//vec3 SkyLightExpose(const vec3 color, float exposure)
//{
//    return 2.0 / (1.0 + exp(-exposure * color)) - 1.0;
//}

vec3 HosekWilkie(float gamma, float cos_gamma, float cos_theta, const vec3 coeffs[10])
{
    vec3 A = coeffs[0];
    vec3 B = coeffs[1];
    vec3 C = coeffs[2];
    vec3 D = coeffs[3];
    vec3 E = coeffs[4];
    vec3 F = coeffs[5];
    vec3 G = coeffs[6];
    vec3 H = coeffs[7];
    vec3 I = coeffs[8];
    vec3 Z = coeffs[9];

    float cos_gamma2 = cos_gamma * cos_gamma;
    vec3 I2 = I * I;
    vec3 chi = ((1.0 + cos_gamma2) / pow(1.0 + I2 - 2.0 * (cos_gamma * I), vec3(1.5, 1.5, 1.5)));
    vec3 color =  Z * ((1.0 + A * exp(B / (cos_theta + 0.01))) * (C + D * exp(E * gamma) + F * cos_gamma2 + G * chi + H * sqrt(cos_theta)));
    return SkyLightExpose(color);
}

vec3 HosekWilkie(const highp vec3 uv, const highp vec3 lightDir, const vec3 coeffs[10])
{
    vec3 A = coeffs[0];
    vec3 B = coeffs[1];
    vec3 C = coeffs[2];
    vec3 D = coeffs[3];
    vec3 E = coeffs[4];
    vec3 F = coeffs[5];
    vec3 G = coeffs[6];
    vec3 H = coeffs[7];
    vec3 I = coeffs[8];
    vec3 Z = coeffs[9];

    highp vec3 V = normalize(uv);
    vec3 N = normalize(lightDir);
//    V.y = abs(V.y);
//    if (abs(V.y) < 0.001) V.y = 0.001;
    float cos_theta = clamp(V.y, 0.0, 1.0);
    float cos_gamma = dot(V, N);
    float gamma = acos(cos_gamma);
    float cos_gamma2 = cos_gamma * cos_gamma;
    vec3 I2 = I * I;

    vec3 chi = ((1.0 + cos_gamma2) / pow(1.0 + I2 - 2.0 * (cos_gamma * I), vec3(1.5, 1.5, 1.5)));
    vec3 color =  Z * ((1.0 + A * exp(B / (cos_theta + 0.01))) * (C + D * exp(E * gamma) + F * cos_gamma2 + G * chi + H * sqrt(cos_theta)));
    return SkyLightExpose(color);
}
