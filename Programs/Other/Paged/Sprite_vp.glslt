// This source file is part of "glue project". Created by Andrey Vasiliev

#ifndef VERSION
#ifndef GL_ES
#version 330 core
#define VERSION 330
#else
#version 100
#define VERSION 100
#endif
#endif

#include "header.vert"

uniform mat4 worldViewProj;
#ifdef FADE
uniform vec3  camPos;
uniform float fadeGap;
uniform float invisibleDist;
#endif
uniform float uScroll;
uniform float vScroll;
uniform vec4  preRotatedQuad[4];

in vec4 vertex;
in vec4 normal;
in vec4 colour;
in vec4 uv0;

out vec4 oUV;
out vec4 oColour;
out float oFogCoord;

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
