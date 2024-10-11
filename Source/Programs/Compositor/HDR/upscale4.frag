// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

// Standard box filtering
vec3 UpsampleBox(const sampler2D tex, const vec2 uv)
{
    vec3 o;
    o =  textureLodOffset(tex, uv, 0.0, ivec2(-1, -1)).rgb;
    o += textureLodOffset(tex, uv, 0.0, ivec2( 1, -1)).rgb;
    o += textureLodOffset(tex, uv, 0.0, ivec2(-1,  1)).rgb;
    o += textureLodOffset(tex, uv, 0.0, ivec2( 1,  1)).rgb;

    return o * 0.25;
}

out vec3 FragColor;
void main()
{
    FragColor.rgb = UpsampleBox(RT, 0.5 * gl_FragCoord.xy / vec2(textureSize(RT, 0)));
}
