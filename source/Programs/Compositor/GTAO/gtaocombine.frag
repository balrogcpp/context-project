// created by Andrey Vasiliev

#include "header.glsl"
#include "tonemap.glsl"

uniform sampler2D RT;
uniform sampler2D AO;

// https://github.com/asylum2010/Asylum_Tutorials/blob/423e5edfaee7b5ea450a450e65f2eabf641b2482/Media/ShadersGL/gtaocombine.frag#L11
vec3 MultiBounce(float visibility, const vec3 albedo) {
    // Jimenez et al. 2016, "Practical Realtime Strategies for Accurate Indirect Occlusion"
    vec3 a =  2.0404 * albedo - 0.3324;
    vec3 b = -4.7951 * albedo + 0.6417;
    vec3 c =  2.7552 * albedo + 0.6903;

    return max(vec3(visibility, visibility, visibility), ((visibility * a + b) * visibility + c) * visibility);
}

out vec3 FragColor;
void main()
{
    vec2 uv = vec2(gl_FragCoord.xy) / vec2(textureSize(RT, 0));
    uv.y = 1.0 - uv.y;

    vec3 albedo = inverseTonemapSRGB(textureLod(RT, uv, 0.0).rgb);
    float ao = textureLod(AO, uv, 0.0).r;

    FragColor.rgb = tonemap(MultiBounce(ao, albedo));
}
