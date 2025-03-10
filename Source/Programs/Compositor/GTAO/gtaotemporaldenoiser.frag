
#version 330

uniform sampler2D historyBuffer;
uniform sampler2D currIteration;
uniform sampler2D prevDepthBuffer;
uniform sampler2D currDepthBuffer;


uniform mat4 matPrevViewInv;
uniform mat4 matCurrViewInv;
uniform mat4 matPrevView;
uniform mat4 matProj;

uniform vec4 projInfo;
uniform vec4 clipPlanes;

vec4 mulMat4x4Half3(const mat4 m, const vec3 v)
{
    return v.x * m[0] + (v.y * m[1] + (v.z * m[2] + m[3]));
}

float sq(const vec3 x)
{
	return dot(x, x);
}

vec3 GetWorldPosition(const float d, const vec2 spos, const mat4 viewinv)
{
	vec3 vpos;

	// NOTE: depth is linearized
	vpos.z = clipPlanes.x + d * (clipPlanes.y - clipPlanes.x);
	vpos.xy = (spos * projInfo.xy) * vpos.z;
	
	vpos.z = -vpos.z;	// by def

	//vec4 wpos = (viewinv * vpos);
	vec4 wpos = mulMat4x4Half3(viewinv, vpos);
	return wpos.xyz;
}

vec3 GetPrevScreenPosition(const vec3 wpos)
{
	vec4 vpos = mulMat4x4Half3(matPrevView, wpos);
	vec4 cpos = mulMat4x4Half3(matProj, vpos.xyz);

	float d = (-vpos.z - clipPlanes.x) / (clipPlanes.y - clipPlanes.x);
	return vec3(cpos.xy / cpos.w, d);
}

//in vec2 tex;
out vec2 my_FragColor0;
void main()
{
	vec2 tex = gl_FragCoord.xy / vec2(textureSize(currIteration, 0));
	vec2 spos = tex * 2.0 - vec2(1.0, 1.0);

	// unproject to current frame world space
	float currdepth = texture(currDepthBuffer, tex).r;
	vec3 currpos = GetWorldPosition(currdepth, spos, matCurrViewInv);

	// reproject to previous frame screen space
	vec3 tempspos = GetPrevScreenPosition(currpos);
	vec2 temptex = tempspos.xy * 0.5 + vec2(0.5, 0.5);

	// unproject to previous frame world space
	float prevdepth = texture(prevDepthBuffer, temptex).r;
	vec3 prevpos = GetWorldPosition(prevdepth, tempspos.xy, matPrevViewInv);

	// detect disocclusion
	//float dist2 = dot(currpos - prevpos, currpos - prevpos);
	float dist2 = sq(currpos - prevpos);
	
	// fetch values
	float currAO = texture(currIteration, tex).r;
	vec2 accumAO = texture(historyBuffer, temptex).rg;

	float currn = 0.0;
	float prevn = accumAO.y * 6.0;
	float ao = currAO;

	// NOTE: large value causes waving/ghosting, small value causes flickering
	if (dist2 < 1e-4) {
		// no disocclusion, continue convergence
		currn = min(prevn + 1.0, 6.0);
		ao = mix(accumAO.x, currAO, 1.0 / currn);
	}

	my_FragColor0 = vec2(ao, currn / 6.0);
}
