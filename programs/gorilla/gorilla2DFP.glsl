#ifndef GL_ES
#version 120
#else
#version 100
precision highp float;
precision highp int;
precision lowp sampler2D;
#endif

varying	vec4 oUv;
varying	vec4 oColor;

uniform	sampler2D atlas;

void main() 
{
	gl_FragColor = texture2D(atlas, oUv.xy) * oColor;
}