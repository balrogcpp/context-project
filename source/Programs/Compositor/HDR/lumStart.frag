// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform vec4 TexSize;
uniform vec4 ViewportSize;

float luminance(const vec3 col) {
    return dot(col, vec3(0.2126, 0.7152, 0.0722));
}

//const vec2 c_offsets[16] = vec2[16]
//(
//vec2( 0, 0 ), vec2( 1, 0 ), vec2( 0, 1 ), vec2( 1, 1 ),
//vec2( 2, 0 ), vec2( 3, 0 ), vec2( 2, 1 ), vec2( 3, 1 ),
//vec2( 0, 2 ), vec2( 1, 2 ), vec2( 0, 3 ), vec2( 1, 3 ),
//vec2( 2, 2 ), vec2( 3, 2 ), vec2( 2, 3 ), vec2( 3, 3 )
//);
//  https://github.com/OGRECave/ogre-next/blob/v2.3.1/Samples/Media/2.0/scripts/materials/HDR/GLSL/DownScale01_SumLumStart_ps.glsl
in highp vec2 vUV0;
void main()
{
    //Compute how many pixels we have to skip because we can't sample them all
    //e.g we have a 4096x4096 viewport (rt0), and we're rendering to a 64x64 surface
    //We would need 64x64 samples, but we only sample 4x4, therefore we sample one
    //pixel and skip 15, then repeat. We perform:
    //(ViewportResolution / TargetResolution) / 4
    // float lum = Downscale4x4(RT, vUV0);
    vec2 ratio = (TexSize.xy * ViewportSize.zw) * 0.25;
    vec2 tsize = ratio * TexSize.zw;

    float A = luminance(textureLod(RT, vUV0                         , 0.0).rgb) + 0.0001;
    float B = luminance(textureLod(RT, vUV0 + tsize * vec2(1.0, 0.0), 0.0).rgb) + 0.0001;
    float C = luminance(textureLod(RT, vUV0 + tsize * vec2(0.0, 1.0), 0.0).rgb) + 0.0001;
    float D = luminance(textureLod(RT, vUV0 + tsize * vec2(1.0, 1.0), 0.0).rgb) + 0.0001;
    float E = luminance(textureLod(RT, vUV0 + tsize * vec2(2.0, 0.0), 0.0).rgb) + 0.0001;
    float F = luminance(textureLod(RT, vUV0 + tsize * vec2(3.0, 0.0), 0.0).rgb) + 0.0001;
    float G = luminance(textureLod(RT, vUV0 + tsize * vec2(2.0, 1.0), 0.0).rgb) + 0.0001;
    float H = luminance(textureLod(RT, vUV0 + tsize * vec2(3.0, 1.0), 0.0).rgb) + 0.0001;
    float I = luminance(textureLod(RT, vUV0 + tsize * vec2(0.0, 2.0), 0.0).rgb) + 0.0001;
    float J = luminance(textureLod(RT, vUV0 + tsize * vec2(1.0, 2.0), 0.0).rgb) + 0.0001;
    float K = luminance(textureLod(RT, vUV0 + tsize * vec2(0.0, 3.0), 0.0).rgb) + 0.0001;
    float L = luminance(textureLod(RT, vUV0 + tsize * vec2(1.0, 3.0), 0.0).rgb) + 0.0001;
    float M = luminance(textureLod(RT, vUV0 + tsize * vec2(2.0, 2.0), 0.0).rgb) + 0.0001;
    float N = luminance(textureLod(RT, vUV0 + tsize * vec2(3.0, 2.0), 0.0).rgb) + 0.0001;
    float O = luminance(textureLod(RT, vUV0 + tsize * vec2(2.0, 3.0), 0.0).rgb) + 0.0001;
    float P = luminance(textureLod(RT, vUV0 + tsize * vec2(3.0, 4.0), 0.0).rgb) + 0.0001;

    float c = log(A * 1024.0) + log(B * 1024.0) + log(C * 1024.0) + log(D * 1024.0);
    c += log(E * 1024.0) + log(F * 1024.0) + log(G * 1024.0) + log(H * 1024.0);
    c += log(I * 1024.0) + log(J * 1024.0) + log(K * 1024.0) + log(L * 1024.0);
    c += log(M * 1024.0) + log(N * 1024.0) + log(O * 1024.0) + log(P * 1024.0);

    float lum = c * 0.0625; // 0.0625 = 1/16

    FragColor.r = lum;
}
