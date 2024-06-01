// created by Andrey Vasiliev

#include "tonemap.glsl"

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

// https://knarkowicz.wordpress.com/2014/04/16/octahedron-normal-vector-encoding/
// Octahedron-normal vectors
vec2 OctWrap(const vec2 v )
{
    // return (1.0 - abs( v.yx)) * (v.xy >= 0.0 ? 1.0 : -1.0);
    return (1.0 - abs(v.yx)) * vec2(v.x >= 0.0 ? 1.0 : -1.0, v.y >= 0.0 ? 1.0 : -1.0);
}

vec2 Encode(vec3 n)
{
    n /= (abs(n.x) + abs(n.y) + abs(n.z));
    n.xy = n.z >= 0.0 ? n.xy : OctWrap(n.xy);
    n.xy = n.xy * 0.5 + 0.5;
    return n.xy;
}

vec3 Decode(vec2 f)
{
    f = f * 2.0 - 1.0;

    // https://twitter.com/Stubbesaurus/status/937994790553227264
    vec3 n = vec3(f.x, f.y, 1.0 - abs(f.x) - abs(f.y));
    float t = clamp(-n.z, 0.0, 1.0);
    // n.xy += n.xy >= 0.0 ? -t : t;
    n.xy += vec2(n.x >= 0.0 ? -t : t, n.y >= 0.0 ? -t : t);
    return normalize(n);
}

float pack(const vec3 n)
{
    return PackFloat16bit2(encode(n));
}

vec3 unpack(float f)
{
    return decode(UnPackFloat16bit2(f));
}

vec3 toSRGB(const vec3 v) { return sqrt(v); }
vec3 fromSRGB(const vec3 v) { return v * v; }

void EvaluateBuffer(const vec4 color, float depth)
{
#ifdef FORCE_TONEMAP
    FragColor = SafeHDR(vec4(unreal(color.rgb), color.a));
#else
    FragColor = SafeHDR(color);
#endif

#ifdef HAS_MRT
#ifdef MRT_DEPTH
    FragData[MRT_DEPTH].r = depth;
#endif
#endif
}

void EvaluateBuffer(const vec4 color, float depth, const vec3 normal, const vec2 velocity)
{
#ifdef FORCE_TONEMAP
    FragColor = SafeHDR(vec4(unreal(color.rgb), color.a));
#else
    FragColor = SafeHDR(color);
#endif

#ifdef HAS_MRT
#ifdef MRT_DEPTH
    FragData[MRT_DEPTH].rg = vec2(depth, pack(normal));
#endif
#ifdef MRT_VELOCITY
    FragData[MRT_VELOCITY].rg = vec2(velocity);
#endif
#endif
}
