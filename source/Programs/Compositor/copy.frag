// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

in vec2 vUV0;
void main()
{
    FragColor = texture2D(RT, vUV0);
}