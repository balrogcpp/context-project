// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D Lum;
uniform vec3 Exposure;
uniform float timeSinceLast;

float expose(float color, const vec3 exposure) {
    return exposure.x / exp(clamp(color, exposure.y, exposure.z));
}

void main()
{
    ivec2 uv = ivec2(gl_FragCoord.xy * 2.0);
    float c = texelFetch(RT, uv, 0).r;
    c += texelFetch(RT, uv + ivec2(0, 1), 0).r;
    c += texelFetch(RT, uv + ivec2(1, 0), 0).r;
    c += texelFetch(RT, uv + ivec2(1, 1), 0).r;
    float newLum = c * 0.25;

    newLum = expose(newLum, Exposure);
    float oldLum = texelFetch(Lum, ivec2(0, 0), 0).r;

    //Adapt luminicense based 75% per second.
    FragColor.r = mix(newLum, oldLum, pow(0.25, timeSinceLast));
}
