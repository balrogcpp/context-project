// created by Andrey Vasiliev
//? #version 400

#include "sky-lut.glsl"
#include "tonemap.glsl"

// changes precision highp
#include "clouds.glsl"

uniform vec3 LightDir0;
uniform vec4 ViewportSize;


vec2 oct_wrap(const vec2 v) {
	vec2 signVal;
	signVal.x = v.x >= 0.0 ? 1.0 : -1.0;
	signVal.y = v.y >= 0.0 ? 1.0 : -1.0;

	return (1.0 - abs(v.yx)) * signVal;
}

vec2 vec3_to_oct(in vec3 e) {
	e /= abs(e.x) + abs(e.y) + abs(e.z);
	e.xy = e.z >= 0.0 ? e.xy : oct_wrap(e.xy);

	vec2 n;
	n.y = e.y * 0.5 + 0.5;
	n.x = e.x * 0.5 + n.y;
	n.y = e.x * -0.5 + n.y;

	return n;
}

// Hemisphere octahedral. Maximizes use of square texture.
// Adapted from https://johnwhite3d.blogspot.com/2017/10/signed-octahedron-normal-encoding.html
vec3 oct_to_vec3(const vec2 e) {
	vec3 n;
	n.x = (e.x - e.y);
	n.y = (e.x + e.y) - 1.0;
	n.z = 1.0 - abs(n.x) - abs(n.y);
	n.xy = n.z >= 0.0 ? n.xy : oct_wrap(n.xy);

	return normalize(n);
}


out vec3 FragColor;
void main()
{
	vec3 dir = oct_to_vec3(gl_FragCoord.xy * ViewportSize.zw).xzy;
	vec3 ldir = -normalize(LightDir0);
    vec3 color = sky(dir, ldir);
    FragColor = tonemap(color);
}
