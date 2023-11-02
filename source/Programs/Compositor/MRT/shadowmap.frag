// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D ShadowMap0;
uniform sampler2D ShadowMap1;
uniform sampler2D ShadowMap2;
uniform sampler2D ShadowMap3;

in highp vec2 vUV0;
void main()
{
    if (vUV0.x >= 0.0 && vUV0.y >= 0.0 && vUV0.x <= 0.5 && vUV0.y <= 0.5)
        FragColor.r = texture2D(ShadowMap0, vUV0 * 2.0).r;
    else if (vUV0.x <= 1.0 && vUV0.y >= 0.0 && vUV0.x > 0.5 && vUV0.y <= 0.5)
        FragColor.r = texture2D(ShadowMap1, (vUV0 - vec2(0.5, 0.0)) * 2.0).r;
    else if (vUV0.x >= 0.0 && vUV0.y <= 1.0 && vUV0.x <= 0.5 && vUV0.y > 0.5)
        FragColor.r = texture2D(ShadowMap2, (vUV0 - vec2(0.0, 0.5)) * 2.0).r;
    else if (vUV0.x <= 1.0 && vUV0.y <= 1.0 && vUV0.x > 0.5 && vUV0.y > 0.5)
        FragColor.r = texture2D(ShadowMap3, (vUV0 - vec2(0.5, 0.5)) * 2.0).r;
    else
        FragColor.r = 0.0;
}
