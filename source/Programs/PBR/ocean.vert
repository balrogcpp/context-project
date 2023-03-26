// created by Andrey Vasiliev

#ifndef __VERSION__
#ifndef GL_ES
#version 330 core
#define __VERSION__ 330
#else
#version 300 es
#define __VERSION__ 300
#endif
#endif


#include "header.vert"
#include "math.glsl"

in highp vec4 position;

uniform highp mat4 MVPMatrix;
uniform highp mat4 ModelMatrix;

out highp vec3 vPosition;
out vec4 vScreenPosition;


void main()
{
    vec4 model = ModelMatrix * position;
    vPosition = model.xyz / model.w;

    gl_Position = MVPMatrix * position;

    vScreenPosition = gl_Position;
}
