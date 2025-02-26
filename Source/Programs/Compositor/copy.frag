// created by Andrey Vasiliev
//? #version 400

uniform mediump sampler2D RT;

out vec4 FragColor;
void main()
{
    FragColor = texelFetch(RT, ivec2(gl_FragCoord.xy), 0);
}
