// created by Andrey Vasiliev

#include "header.glsl"

#if MAX_SHADOW_TEXTURES > 0
uniform mediump sampler2D ShadowMap0;
#endif
#if MAX_SHADOW_TEXTURES > 1
uniform mediump sampler2D ShadowMap1;
#endif
#if MAX_SHADOW_TEXTURES > 2
uniform mediump sampler2D ShadowMap2;
#endif
#if MAX_SHADOW_TEXTURES > 3
uniform mediump sampler2D ShadowMap3;
#endif
#if MAX_SHADOW_TEXTURES > 4
uniform mediump sampler2D ShadowMap4;
#endif
#if MAX_SHADOW_TEXTURES > 5
uniform mediump sampler2D ShadowMap5;
#endif
#if MAX_SHADOW_TEXTURES > 6
uniform mediump sampler2D ShadowMap6;
#endif
#if MAX_SHADOW_TEXTURES > 7
uniform mediump sampler2D ShadowMap7;
#endif

in highp vec2 vUV0;
void main()
{
#if MAX_SHADOW_TEXTURES > 0
    if (vUV0.x <= 0.5 && vUV0.y <= 0.5)
        FragColor.r = texture2D(ShadowMap0, vUV0 * 2.0).x;
#if MAX_SHADOW_TEXTURES > 1
    else if (vUV0.x > 0.5 && vUV0.y <= 0.5)
        FragColor.r = texture2D(ShadowMap1, (vUV0 - vec2(0.5, 0.0)) * 2.0).x;
#endif
#if MAX_SHADOW_TEXTURES > 2
    else if (vUV0.x <= 0.5 && vUV0.y > 0.5)
        FragColor.r = texture2D(ShadowMap2, (vUV0 - vec2(0.0, 0.5)) * 2.0).x;
#endif
#if MAX_SHADOW_TEXTURES > 3
#if MAX_SHADOW_TEXTURES == 4
    else if (vUV0.x > 0.5 && vUV0.y > 0.5)
        FragColor.r = texture2D(ShadowMap3, (vUV0 - vec2(0.5, 0.5)) * 2.0).x;
#else
    else if (vUV0.x > 0.5 && vUV0.y > 0.5) {
#if MAX_SHADOW_TEXTURES > 3
        if (vUV0.x <= 0.75 && vUV0.y <= 0.75)
            FragColor.r = texture2D(ShadowMap3, (vUV0 - vec2(0.5, 0.5)) * 4.0).x;
#endif
#if MAX_SHADOW_TEXTURES > 4
        else if (vUV0.x > 0.75 && vUV0.y <= 0.75)
            FragColor.r = texture2D(ShadowMap4, (vUV0 - vec2(0.75, 0.5)) * 4.0).x;
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (vUV0.x <= 0.75 && vUV0.y > 0.75)
            FragColor.r = texture2D(ShadowMap5, (vUV0 - vec2(0.5, 0.75)) * 4.0).x;
#endif
#if MAX_SHADOW_TEXTURES > 5
        else if (vUV0.x > 0.75 && vUV0.y > 0.75)
            FragColor.r = texture2D(ShadowMap6, (vUV0 - vec2(0.75, 0.75)) * 4.0).x;
#endif
    }
#endif // MAX_SHADOW_TEXTURES != 4

#endif // MAX_SHADOW_TEXTURES > 3
    else
        FragColor.r = 1.0;
#else
    FragColor.r = 1.0;
#endif // MAX_SHADOW_TEXTURES > 0
}
