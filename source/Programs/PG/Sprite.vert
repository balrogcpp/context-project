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

#include "header.glsl"

uniform mat4 worldViewProj;
#ifdef FADE
uniform vec3  camPos;
uniform float fadeGap;
uniform float invisibleDist;
#endif
uniform float uScroll;
uniform float vScroll;
uniform vec4  preRotatedQuad[4];

attribute vec4 vertex;
attribute vec4 normal;
attribute vec4 colour;
attribute vec4 uv0;

varying vec4 oUV;
varying vec4 oColour;
varying float oFogCoord;

void main()
{
    //Face the camera
	vec4 vCenter = vec4( vertex.x, vertex.y, vertex.z, 1.0 );
	vec4 vScale = vec4( normal.x, normal.y, normal.x , 1.0 );
	gl_Position = worldViewProj * (vCenter + (preRotatedQuad[int(normal.z)] * vScale));

	//Color
	oColour = colour;

    //Fade out in the distance
#ifdef FADE
	vec4 position = vertex;
	float dist = distance(camPos.xz, position.xz);
	oColour.w = (invisibleDist - dist) / fadeGap;
#endif

    //UV scroll
	oUV = uv0;
	oUV.x += uScroll;
	oUV.y += vScroll;

    //Fog
	oFogCoord = gl_Position.z;
}
