// created by Andrey Vasiliev

#ifndef __VERSION__
#if defined(OGRE_GLSL)
#version 330 core
#define __VERSION__ 330
#elif defined(OGRE_GLSLES)
#version 300 es
#define __VERSION__ 300
#endif
#endif

#include "header.glsl"

uniform highp mat4 WorldViewProjMatrix;
uniform highp mat4 WorldViewProjPrev;
#ifdef FADE
uniform vec3  camPos;
uniform float fadeGap;
uniform float invisibleDist;
#endif
uniform float uScroll;
uniform float vScroll;
uniform vec4  preRotatedQuad[4];

in highp vec4 vertex;
in highp vec4 normal;
in highp vec4 colour;
in highp vec4 uv0;
out highp vec4 oUV;
out mediump vec4 oColour;
out mediump float oFogCoord;
out mediump vec4 vScreenPosition;
out mediump vec4 vPrevScreenPosition;
void main()
{
    //Face the camera
	vec4 vCenter = vec4( vertex.x, vertex.y, vertex.z, 1.0 );
	vec4 vScale = vec4( normal.x, normal.y, normal.x , 1.0 );
	vec4 position = vCenter + (preRotatedQuad[int(normal.z)] * vScale);
	gl_Position = mul(WorldViewProjMatrix, position);
	vScreenPosition = gl_Position;
	vPrevScreenPosition = mul(WorldViewProjPrev, position);

	//Color
	oColour = colour;

    //Fade out in the distance
#ifdef FADE
	float dist = distance(camPos.xz, vertex.xz);
	oColour.w = (invisibleDist - dist) / fadeGap;
#endif

    //UV scroll
	oUV = uv0;
	oUV.x += uScroll;
	oUV.y += vScroll;

    //Fog
	oFogCoord = gl_Position.z;
}
