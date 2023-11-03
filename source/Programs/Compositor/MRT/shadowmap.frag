// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D ShadowMap0;
uniform sampler2D ShadowMap1;
uniform sampler2D ShadowMap2;
uniform sampler2D ShadowMap3;

float Box(sampler2D tex, const vec2 uv)
{
    vec2 tsize = 1.0 / vec2(textureSize(tex, 0));
    float c = 0.0;
    for (int x = -2; x < 1; x++)
    for (int y = -2; y < 1; y++) {
        c += texture2D(tex, vec2(uv + vec2(x, y) * tsize)).x / 9.0;
    }

    return c;
}

in highp vec2 vUV0;
void main()
{
    if (vUV0.x >= 0.0 && vUV0.x <= 0.5 && vUV0.y >= 0.0  && vUV0.y <= 0.5)
        FragColor.r = Box(ShadowMap0, vUV0 * 2.0);
    else if (vUV0.x <= 1.0 && vUV0.x > 0.5 && vUV0.y >= 0.0  && vUV0.y <= 0.5)
        FragColor.r = Box(ShadowMap1, (vUV0 - vec2(0.5, 0.0)) * 2.0);
    else if (vUV0.x >= 0.0 && vUV0.x <= 0.5 && vUV0.y <= 1.0 && vUV0.y > 0.5)
        FragColor.r = Box(ShadowMap2, (vUV0 - vec2(0.0, 0.5)) * 2.0);
    else if (vUV0.x <= 1.0 && vUV0.x > 0.5 && vUV0.y <= 1.0 && vUV0.y > 0.5)
        FragColor.r = Box(ShadowMap3, (vUV0 - vec2(0.5, 0.5)) * 2.0);
    else
        FragColor.r = 1.0;
}
