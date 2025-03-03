/*
Film Grain post-process shader v1.0	
Martins Upitis (martinsh) devlog-martinsh.blogspot.com
2013

The shader does not yet have a genuine film grain look so I will continue developing it. 
I have to try out different noise algorithms.

--------------------------
This work is licensed under a Creative Commons Attribution 3.0 Unported License.
So you are free to share, modify and adapt it for your needs, and even use it for commercial use.
I would also love to hear about a project you are using it.

Have fun,
Martins
--------------------------

credits:
Perlin noise shader by toneburst:
http://machinesdontcare.wordpress.com/2009/06/25/3d-perlin-noise-sphere-vertex-shader-sourcecode/

*/

//? #version 400

const float permTexUnit = 1.0/256.0;		// Perm texture texel-size
const float permTexUnitHalf = 0.5/256.0;	// Half perm texture texel-size
const float grainamount = 0.03; //grain amount

float timer = 0.0;


//a random texture generator, but you can also use a pre-computed perturbation texture
vec4 rnm(const vec2 tc) 
{
    float noise =  sin(dot(tc + vec2(timer, timer), vec2(12.9898, 78.233))) * 43758.5453;

	float noiseR =  fract(noise);
	float noiseG =  fract(noise * 1.2154); 
	float noiseB =  fract(noise * 1.3453);
	float noiseA =  fract(noise * 1.3647);
	
	return vec4(noiseR, noiseG, noiseB, noiseA) * 2.0 - 1.0;
}

float fade(const float t) {
	return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

vec3 fade(const vec3 t) {
	return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float pnoise3D(const vec3 p)
{
	vec3 pi = permTexUnit*floor(p)+permTexUnitHalf; // Integer part, scaled so +1 moves permTexUnit texel
	// and offset 1/2 texel to sample texel centers
	vec3 pf = fract(p);     // Fractional part for interpolation

	// Noise contributions from (x=0, y=0), z=0 and z=1
	float perm00 = rnm(pi.xy).a ;
	vec3  grad000 = rnm(vec2(perm00, pi.z)).rgb * 4.0 - 1.0;
	float n000 = dot(grad000, pf);
	vec3  grad001 = rnm(vec2(perm00, pi.z + permTexUnit)).rgb * 4.0 - 1.0;
	float n001 = dot(grad001, pf - vec3(0.0, 0.0, 1.0));

	// Noise contributions from (x=0, y=1), z=0 and z=1
	float perm01 = rnm(pi.xy + vec2(0.0, permTexUnit)).a ;
	vec3  grad010 = rnm(vec2(perm01, pi.z)).rgb * 4.0 - 1.0;
	float n010 = dot(grad010, pf - vec3(0.0, 1.0, 0.0));
	vec3  grad011 = rnm(vec2(perm01, pi.z + permTexUnit)).rgb * 4.0 - 1.0;
	float n011 = dot(grad011, pf - vec3(0.0, 1.0, 1.0));

	// Noise contributions from (x=1, y=0), z=0 and z=1
	float perm10 = rnm(pi.xy + vec2(permTexUnit, 0.0)).a ;
	vec3  grad100 = rnm(vec2(perm10, pi.z)).rgb * 4.0 - 1.0;
	float n100 = dot(grad100, pf - vec3(1.0, 0.0, 0.0));
	vec3  grad101 = rnm(vec2(perm10, pi.z + permTexUnit)).rgb * 4.0 - 1.0;
	float n101 = dot(grad101, pf - vec3(1.0, 0.0, 1.0));

	// Noise contributions from (x=1, y=1), z=0 and z=1
	float perm11 = rnm(pi.xy + vec2(permTexUnit, permTexUnit)).a ;
	vec3  grad110 = rnm(vec2(perm11, pi.z)).rgb * 4.0 - 1.0;
	float n110 = dot(grad110, pf - vec3(1.0, 1.0, 0.0));
	vec3  grad111 = rnm(vec2(perm11, pi.z + permTexUnit)).rgb * 4.0 - 1.0;
	float n111 = dot(grad111, pf - vec3(1.0, 1.0, 1.0));

	// Blend contributions along x
	vec4 n_x = mix(vec4(n000, n001, n010, n011), vec4(n100, n101, n110, n111), fade(pf.x));

	// Blend contributions along y
	vec2 n_xy = mix(n_x.xy, n_x.zw, fade(pf.y));

	// Blend contributions along z
	float n_xyz = mix(n_xy.x, n_xy.y, fade(pf.z));

	// We're done, return the final noise value.
	return n_xyz;
}

vec3 grainHigh(const vec3 col, const highp vec2 texCoord, const float time) 
{
    timer = time;

	vec3 noise;
    noise.r = pnoise3D(vec3(texCoord, 0.0));
    noise.g = pnoise3D(vec3(texCoord, 1.0));
    noise.b = pnoise3D(vec3(texCoord, 2.0));

    return col + noise * grainamount;
}

vec3 grainLow(const vec3 color, const highp vec2 uv, const float time) {
    float noise = (fract(sin(dot(uv * time, vec2(12.9898, 78.233))) * 43758.5453) - 0.5) * 2.0;
    return color + noise * 0.03;
}
