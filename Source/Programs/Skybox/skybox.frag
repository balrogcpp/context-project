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
    vec3 color = sky(normalize(vUV0), -normalize(LightDir0));
    FragColor = tonemap(color);
}
