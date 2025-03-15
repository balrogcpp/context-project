// created by Andrey Vasiliev
//? #version 400

#include "sky-lut.glsl"
#include "clouds.glsl"
#include "tonemap.glsl"

uniform vec3 LightDir0;


in highp vec3 vUV0;
out vec3 FragColor;
void main()
{
    vec3 color = sky(normalize(vUV0), -normalize(LightDir0)).rgb;
    FragColor = tonemap(color);

    //vec3 uv = vec3(gl_FragCoord.xy, 0.0) / vec3(textureSize(perlworlnoise, 0));
    //uv /= 5.0;
    //uv.z += TIME * 0.1;
    //FragColor = texture(perlworlnoise, uv).rgb;
}
