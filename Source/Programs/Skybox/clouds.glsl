// created by Andrey Vasiliev
//? #version 400

// Cloud Raymarching based on: A. Schneider. “The Real-Time Volumetric Cloudscapes Of Horizon: Zero Dawn”. ACM SIGGRAPH. Los Angeles, CA: ACM SIGGRAPH, 2015. Web. 26 Aug. 2015.

#define PI 3.14159265359

uniform sampler3D worlnoise;
uniform sampler3D perlworlnoise;
uniform sampler2D weathermap;

uniform float TIME;
uniform vec3 HosekSamples[3];

const float LIGHT0_ENERGY = 1000.0;
const vec2 wind_direction = vec2(1.0, 0.0);
const float wind_speed = 1.0;
const float _density = 0.05;
const float cloud_coverage = 0.2;
const float _time_offset = 0.0;


// Approximately earth sizes
const float g_radius = 6000000.0; //ground radius
const float sky_b_radius = 6001500.0;//bottom of cloud layer
const float sky_t_radius = 6004000.0;//top of cloud layer


// From: https://www.shadertoy.com/view/4sfGzS credit to iq
float hash(const vec3 p) {
	vec3 t = fract(p * 0.3183099 + 0.1) * 17.0;
	return fract(t.x * t.y * t.z * (t.x + t.y + t.z));
}

// Utility function that maps a value from one range to another. 
float remap(const float originalValue, const float originalMin,  const float originalMax,  const float newMin, const float newMax) {
	return newMin + (((originalValue - originalMin) / (originalMax - originalMin)) * (newMax - newMin));
}

// Phase const float
float henyey_greenstein(const float cos_theta, const float g) {
	float g2 = g * g;
	return 0.0795774715459 * (1.0 - g2) / (pow(1.0 + g2 - 2.0 * g * cos_theta, 1.5));
}

float GetHeightFractionForPoint(const float inPosition) { 
	float height_fraction = (inPosition -  sky_b_radius) / (sky_t_radius - sky_b_radius); 
	return clamp(height_fraction, 0.0, 1.0);
}

vec4 mixGradients(const float cloudType){
	const vec4 STRATUS_GRADIENT = vec4(0.02, 0.05, 0.09, 0.11);
	const vec4 STRATOCUMULUS_GRADIENT = vec4(0.02, 0.2, 0.48, 0.625);
	const vec4 CUMULUS_GRADIENT = vec4(0.01, 0.0625, 0.78, 1.0);
	float stratus = 1.0 - clamp(cloudType * 2.0, 0.0, 1.0);
	float stratocumulus = 1.0f - abs(cloudType - 0.5) * 2.0;
	float cumulus = clamp(cloudType - 0.5, 0.0, 1.0) * 2.0;
	return STRATUS_GRADIENT * stratus + STRATOCUMULUS_GRADIENT * stratocumulus + CUMULUS_GRADIENT * cumulus;
}

float densityHeightGradient(const float heightFrac, const float cloudType) {
	vec4 cloudGradient = mixGradients(cloudType);
	return smoothstep(cloudGradient.x, cloudGradient.y, heightFrac) - smoothstep(cloudGradient.z, cloudGradient.w, heightFrac);
}

float intersectSphere(const vec3 pos, const vec3 dir, const float r) {
    float a = dot(dir, dir);
    float b = 2.0 * dot(dir, pos);
    float c = dot(pos, pos) - (r * r);
	float d = sqrt((b * b) - 4.0 * a * c);
	float p = -b - d;
	float p2 = -b + d;
    return max(p, p2) / (2.0 * a);
}


// Returns density at a given point
// Heavily based on method from Schneider
float density(const vec3 pip, const vec3 weather, const float mip) {
	vec3 p = pip;
	float height_fraction = GetHeightFractionForPoint(length(p));
	p.xz += TIME * 20.0 * wind_direction * wind_speed * 0.6;
	vec4 n = textureLod(perlworlnoise, p.xyz * 0.00008, mip - 2.0);
	float fbm = n.g * 0.625 + n.b * 0.25 + n.a * 0.125;
	float g = densityHeightGradient(height_fraction, weather.r);
	float base_cloud = remap(n.r, -(1.0 - fbm), 1.0, 0.0, 1.0);
	float weather_coverage = cloud_coverage * weather.b;
	base_cloud = remap(base_cloud * g, 1.0 - weather_coverage, 1.0, 0.0, 1.0);
	base_cloud *= weather_coverage;
	p.xz -= TIME * wind_direction * 40.0;
	p.y -= TIME * 40.0;
	vec3 hn = textureLod(worlnoise, p * 0.001, mip).rgb;
	float hfbm = hn.r * 0.625 + hn.g * 0.25 + hn.b * 0.125;
	hfbm = mix(hfbm, 1.0 - hfbm, clamp(height_fraction * 4.0, 0.0, 1.0));
	base_cloud = remap(base_cloud, hfbm * 0.4 * height_fraction, 1.0, 0.0, 1.0);
	return pow(clamp(base_cloud, 0.0, 1.0), (1.0 - height_fraction) * 0.8 + 0.5);
}

vec4 march(const vec3 pos, const vec3 end, const vec3 dir, const float ss, const vec3 ldir, const int depth) {
	const vec3 RANDOM_VECTORS[6] = vec3[](
		vec3( 0.38051305,  0.92453449, -0.02111345),
		vec3(-0.50625799, -0.03590792, -0.86163418),
		vec3(-0.32509218, -0.94557439,  0.01428793),
		vec3( 0.09026238, -0.27376545,  0.95755165),
		vec3( 0.28128598,  0.42443639, -0.86065785),
		vec3(-0.16852403,  0.14748697,  0.97460106)
	);

	float T = 1.0;
	float alpha = 0.0;
	//float ss = length(dir);
	//dir = normalize(dir);
	vec3 p = pos + dir * hash(pos * 10.0) * ss;
	const float t_dist = sky_t_radius - sky_b_radius;
	float lss = (t_dist / 36.0);
	vec3 L = vec3(0.0, 0.0, 0.0);
	int count = 0;
	float t = 1.0;
	float costheta = dot(ldir, dir);
	// Stack multiple phase functions to emulate some backscattering
	float phase = max(max(henyey_greenstein(costheta, 0.6), henyey_greenstein(costheta, (0.4 - 1.4 * ldir.y))), henyey_greenstein(costheta, -0.2));
	// Precalculate sun and ambient colors
	// This should really come from a uniform or texture for performance reasons
	//vec3 atmosphere_sun = atmosphere(ldir, ldir) * LIGHT0_ENERGY * 0.1;
	//vec3 atmosphere_ambient = atmosphere(vec3(1.414214, 1.414214, 0.0), ldir);
	//vec3 atmosphere_ground = atmosphere(vec3(1.414214, -1.414214, 0.0), ldir);
	vec3 atmosphere_sun = HosekSamples[0] * LIGHT0_ENERGY * 0.1;
	vec3 atmosphere_ambient = HosekSamples[1];
	vec3 atmosphere_ground = HosekSamples[2];
	
	const float weather_scale = 0.00006;
	float time = TIME * 0.001 + 0.005 * _time_offset;
	vec2 weather_pos = time * wind_direction * wind_speed;
	
	for (int i = 0; i < depth; i++) {
		p += dir * ss;
		vec3 weather_sample = texture(weathermap, p.xz * weather_scale + 0.5 + weather_pos).xyz;
		float height_fraction = GetHeightFractionForPoint(length(p));

		t = density(p, weather_sample, 0.0);
		float dt = exp(-_density * t * ss);
		T *= dt;
		vec3 lp = p;
		float lt = 1.0;
		float cd = 0.0;

		if (t > 0.0) { //calculate lighting, but only when we are in the cloud
			float lheight_fraction = 0.0;
			for (int j = 0; j < 6; j++) {
				lp += (ldir + RANDOM_VECTORS[j] * float(j)) * lss;
				lheight_fraction = GetHeightFractionForPoint(length(lp));
				vec3 lweather = texture(weathermap, lp.xz * weather_scale + 0.5 + weather_pos).xyz;
				lt = density(lp, lweather, float(j));
				cd += lt;
			}
			
			// Take a single distant sample
			lp = p + ldir * 18.0 * lss;
			lheight_fraction = GetHeightFractionForPoint(length(lp));
			vec3 lweather = texture(weathermap, lp.xz * weather_scale + 0.5).xyz;
			lt = pow(density(lp, lweather, 5.0), (1.0 - lheight_fraction) * 0.8 + 0.5);
			cd += lt;
			
			// captures the direct lighting from the sun
			float beers = exp(-_density * cd * lss);
			float beers2 = exp(-_density * cd * lss * 0.25) * 0.7;
			float beers_total = max(beers, beers2);

			vec3 ambient = mix(atmosphere_ground, vec3(1.0, 1.0, 1.0), smoothstep(0.0, 1.0, height_fraction)) * _density * mix(atmosphere_ambient, vec3(1.0, 1.0, 1.0), 0.4) * (ldir.y);
			alpha += (1.0 - dt) * (1.0 - alpha);
			L += (ambient + beers_total * atmosphere_sun * phase * alpha) * T * t;
		}
	}

	alpha = clamp(alpha, 0.0, 1.0);
	return vec4(L, alpha);
}

// https://www.shadertoy.com/view/slSXRW
vec3 sunWithBloom(const vec3 rayDir, const vec3 sunDir) {
	const float sun_disk_scale = 2.0;
    float sunSolidAngle = sun_disk_scale * 0.53 * PI / 180.0;
    float minSunCosTheta = cos(sunSolidAngle);

    float cosTheta = dot(rayDir, sunDir);
    if (cosTheta >= minSunCosTheta) return vec3(1.0, 1.0, 1.0);

    float offset = minSunCosTheta - cosTheta;
    float gaussianBloom = exp(-offset * 50000.0) * 0.5;
    float invBloom = 1.0 / (0.02 + offset * 300.0) * 0.01;
    return vec3(gaussianBloom + invBloom);
}

vec3 sky(const vec3 dir, const vec3 ldir) {
	vec3 col;

	if (dir.y > 0.0) {
		vec3 camPos = vec3(0.0, g_radius, 0.0);
		vec3 start = camPos + dir * intersectSphere(camPos, dir, sky_b_radius);
		vec3 end = camPos + dir * intersectSphere(camPos, dir, sky_t_radius);

		float shelldist = length(end - start);

		// Take fewer steps towards horizon
		float steps = (mix(96.0, 54.0, clamp(dot(dir, vec3(0.0, 1.0, 0.0)), 0.0, 1.0)));
		vec3 raystep = dir;// * shelldist / steps;
		vec4 volume = march(start, end, raystep, shelldist / steps, ldir, int(steps));
		vec3 background = atmosphere(dir, ldir);
		vec3 sunLum = sunWithBloom(dir, ldir);
		// Use smoothstep to limit the effect, so it drops off to actual zero.
		sunLum = smoothstep(0.002, 1.0, sunLum);
		background += sunLum;

		// Draw cloud shape
		col = background * (1.0 - volume.a) + volume.xyz;

		// Blend distant clouds into the sky
		col = mix(clamp(col, 0.0, 1.0), clamp(background, 0.0, 1.0), smoothstep(0.6, 1.0, 1.0 - dir.y));
	} else {
		col = atmosphere(dir, ldir);
	}
	
	return col;
}
