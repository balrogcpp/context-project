// created by Andrey Vasiliev

#ifndef MRT_GLSL
#define MRT_GLSL
#ifdef OGRE_FRAGMENT_SHADER

#ifdef HAS_MRT
uniform float FarClipDistance;
uniform float NearClipDistance;
#endif

#ifdef FORCE_TONEMAP
#include "tonemap.glsl"
#endif

// https://community.khronos.org/t/glsl-packing-a-normal-in-a-single-float/52039/13
//Thanks @paveltumik for the original code in comments

//pack:	f1=(f1+1)*0.5; f2=(f2+1)*0.5; res=floor(f1*1000)+f2;
float PackFloat16bit2(const vec2 src)
{
    return floor((src.x + 1.0) * 0.5 * 100.0) + ((src.y + 1.0) * 0.4);
}

//unpack:	f2=frac(res);	f1=(res-f2)/1000;	f1=(f1-0.5)*2;f2=(f2-0.5)*2;
vec2 UnPackFloat16bit2(float src)
{
    float fFrac = fract(src);
    return vec2(((src - fFrac) / 100.0 - 0.5) * 2.0, (fFrac - 0.4) * 2.5);
}

// https://aras-p.info/texts/CompactNormalStorage.html
// Method #4: Spheremap Transform
vec2 encode(const vec3 n)
{
    float p = sqrt(n.z * 8.0 + 8.0);
    return vec2(vec2(n.x, n.y) / p + 0.5);
}

vec3 decode(const vec2 enc)
{
    vec2 fenc = enc * 4.0 - 2.0;
    float f = dot(fenc, fenc);
    float g = sqrt(1.0 - f / 4.0);
    return vec3(fenc * g, 1.0 - f / 2.0);
}

float pack(const vec3 n)
{
    return PackFloat16bit2(encode(n));
}

vec3 unpack(float f)
{
    return decode(UnPackFloat16bit2(f));
}

void EvaluateBuffer(const vec4 color)
{
#ifdef FORCE_TONEMAP
    FragColor = SafeHDR(vec4(unreal(color.rgb), color.a));
#else
    FragColor = SafeHDR(color);
#endif

#ifdef HAS_MRT
    FragData[MRT_DEPTH].r = (gl_FragCoord.z / gl_FragCoord.w - NearClipDistance) / (FarClipDistance - NearClipDistance);
#endif
}

void EvaluateBuffer(const vec4 color, const vec3 normal, const vec2 velocity, float roughness)
{
#ifdef FORCE_TONEMAP
    FragColor = SafeHDR(vec4(unreal(color.rgb), color.a));
#else
    FragColor = SafeHDR(color);
#endif

#ifdef HAS_MRT
    FragData[MRT_DEPTH].rg = vec2((gl_FragCoord.z / gl_FragCoord.w - NearClipDistance) / (FarClipDistance - NearClipDistance), pack(normal));
    FragData[MRT_VELOCITY].rgb = vec3(velocity, roughness);
#endif
}

#endif // OGRE_FRAGMENT_SHADER
#endif // MRT_GLSL
