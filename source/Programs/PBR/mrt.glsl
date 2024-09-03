// created by Andrey Vasiliev

//#include "tonemap.glsl"

// Clamp HDR value within a safe range
//#define SafeHDR(x) min(x, 65504.0)
//
//vec3 toSRGB(const vec3 v) { return sqrt(v); }
//vec3 fromSRGB(const vec3 v) { return v * v; }
//
//void EvaluateBuffer(const vec4 color, float depth)
//{
//#ifdef FORCE_TONEMAP
//    FragColor = SafeHDR(vec4(unreal(color.rgb), color.a));
//#else
//    FragColor = SafeHDR(color);
//#endif
//
//#ifdef HAS_MRT
//#ifdef MRT_DEPTH
//    FragData[MRT_DEPTH].r = depth;
//#endif
//#endif
//}

//void EvaluateBuffer(const vec4 color, float depth, const vec3 normal, const vec2 velocity)
//{
//#ifdef FORCE_TONEMAP
//    FragColor = SafeHDR(vec4(unreal(color.rgb), color.a));
//#else
//    FragColor = SafeHDR(color);
//#endif
//
//#ifdef HAS_MRT
//#ifdef MRT_DEPTH
//    FragData[MRT_DEPTH].rg = vec2(depth, pack(normal));
//#endif
//#ifdef MRT_VELOCITY
//    FragData[MRT_VELOCITY].rg = vec2(velocity);
//#endif
//#endif
//}
