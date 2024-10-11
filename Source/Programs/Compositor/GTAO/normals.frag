// created by Andrey Vasiliev
// based on https://github.com/OGRECave/ogre/blob/v13.6.4/Samples/Media/DeferredShadingMedia/ssao_ps.glsl

#include "header.glsl"
#include "math.glsl"

uniform sampler2D DepthTex;
uniform highp vec4 ZBufferParams;

vec3 GetCameraVec(const vec2 uv)
{
    // Returns the vector from camera to the specified position on the camera plane (uv argument), located one unit away from the camera
    // This vector is not normalized.
    // The nice thing about this setup is that the returned vector from this function can be simply multiplied with the linear depth to get pixel's position relative to camera position.
    // This particular function does not account for camera rotation or position or FOV at all (since we don't need it for AO)
    // TODO: AO is dependent on FOV, this function is not!
    // The outcome of using this simplified function is that the effective AO range is larger when using larger FOV
    // Use something more accurate to get proper FOV-independent world-space range, however you will likely also have to adjust the SSAO constants below
    const float aspect = 1.0;
    return vec3(uv.x * 2.0 - 1.0, -uv.y * 2.0 * aspect + aspect, 1.0);
}

vec3 MinDiff(const vec3 P, const vec3 Pr, const vec3 Pl)
{
    vec3 V1 = Pr - P;
    vec3 V2 = P - Pl;
    return (dot(V1,V1) < dot(V2,V2)) ? V1 : V2;
}

float Linear01Depth(const highp float z)
{
    return 1.0 / (z * ZBufferParams.x + ZBufferParams.y);
}

vec3 getNormal(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
    vec3 P = GetCameraVec(uv) * Linear01Depth(textureLod(tex, uv, 0.0).x);
    vec3 Pr = GetCameraVec(uv + vec2(tsize.x, 0.0)) * Linear01Depth(textureLod(tex, uv + tsize * vec2(1.0, 0.0), 0.0).x);
    vec3 Pl = GetCameraVec(uv + vec2(-tsize.x, 0.0)) * Linear01Depth(textureLod(tex, uv + tsize * vec2(-1.0, 0.0), 0.0).x);
    vec3 Pt = GetCameraVec(uv + vec2(0.0, tsize.y)) * Linear01Depth(textureLod(tex, uv + tsize * vec2(0.0, 1.0), 0.0).x);
    vec3 Pb = GetCameraVec(uv + vec2(0.0, -tsize.y)) * Linear01Depth(textureLod(tex, uv + tsize * vec2(0.0, -1.0), 0.0).x);

    return normalize(cross(MinDiff(P, Pr, Pl), MinDiff(P, Pt, Pb)));
}

out vec3 FragColor;
void main()
{
    vec2 tsize = 1.0 / vec2(textureSize(DepthTex, 0).xy);
    vec2 uv = gl_FragCoord.xy / vec2(textureSize(DepthTex, 0).xy);

    vec3 normal = getNormal(DepthTex, uv, tsize);
    normal.z = -normal.z;
    FragColor.rgb = vec3(normal);
}
