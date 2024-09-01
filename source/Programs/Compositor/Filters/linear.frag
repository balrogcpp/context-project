// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

in highp vec2 vUV0;
void main()
{
    FragColor = textureLod(RT, vUV0, 0.0);
}
