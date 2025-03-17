// created by Andrey Vasiliev
//? #version 400

uniform highp mat4 WorldMatrix;
uniform highp mat4 WorldViewProjMatrix;

in highp vec4 position;
out highp vec3 vPosition;
void main()
{
    highp vec4 world = WorldMatrix * position;
    vPosition = world.xyz / world.w;

    gl_Position = WorldViewProjMatrix * position;
}
