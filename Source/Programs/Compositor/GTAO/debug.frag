// created by Andrey Vasiliev
//? #version 400

uniform sampler2D RT;
uniform sampler2D Depth;
uniform vec4 ZBufferParams;


#define MEDIUMP_FLT_MIN    0.00006103515625
#define KERNEL_RADIUS 3

float Linear01Depth(const highp float z)
{
    return 1.0 / (z * ZBufferParams.x + ZBufferParams.y);
}

float LinearDepth(const highp float z)
{
    return 1.0 / (z * ZBufferParams.z + ZBufferParams.w);
}

float unpack(const vec2 depth) {
    // this is equivalent to (x8 * 256 + y8) / 65535, which gives a value between 0 and 1
    return (depth.x * (256.0 / 257.0) + depth.y * (1.0 / 257.0));
}

// https://github.com/nvpro-samples/gl_ssao/blob/f6b010dc7a05346518cd13d3368d8d830a382ed9/bilateralblur.frag.glsl
float BlurFunction(const vec2 uv, float r, const float center_c, float center_d, inout float w_total)
{
    vec2 occ = texture(RT, uv).rg;
    float c = occ.r;
    float d = occ.g;

    const float sharpness = 1.0;
    const float BlurSigma = float(KERNEL_RADIUS) * 0.5;
    const float BlurFalloff = 1.0 / (2.0 * BlurSigma * BlurSigma);

    float ddiff = (d - center_d) * sharpness;
    float w = exp2(-r * r * BlurFalloff - ddiff * ddiff);
    w_total += w;

    return c * w;
}

float GetAO(const vec2 uv, float center_c, float center_d)
{
    float c_total = center_c;
    float w_total = 1.0;
    vec2 tsize = 1.0 / vec2(textureSize(RT, 0));

    for (int r = 1; r <= KERNEL_RADIUS; ++r)
    {
        c_total += BlurFunction(uv + tsize * float(r), float(r), center_c, center_d, w_total);
    }
    for (int r = 1; r <= KERNEL_RADIUS; ++r)
    {
        c_total += BlurFunction(uv - tsize * float(r), float(r), center_c, center_d, w_total);
    }

    return c_total/w_total;
}


out vec3 FragColor;
void main()
{
    vec2 uv = vec2(gl_FragCoord.xy - vec2(1.5, 1.5)) / vec2(textureSize(Depth, 0));
    uv.y = 1.0 - uv.y;

    float d = LinearDepth(textureLod(Depth, uv, 0.0).x);

#if __VERSION__ > 330
    vec4 ao = textureGather(RT, uv, 0);
    vec4 dg = textureGather(RT, uv, 1);
    vec4 db = textureGather(RT, uv, 2);
#else
    vec3 s01 = textureLodOffset(RT, uv, 0.0, ivec2(0, 1)).rgb;
    vec3 s11 = textureLodOffset(RT, uv, 0.0, ivec2(1, 1)).rgb;
    vec3 s10 = textureLodOffset(RT, uv, 0.0, ivec2(1, 0)).rgb;
    vec3 s00 = textureLodOffset(RT, uv, 0.0, ivec2(0, 0)).rgb;
    vec4 ao = vec4(s01.r, s11.r, s10.r, s00.r);
    vec4 dg = vec4(s01.g, s11.g, s10.g, s00.g);
    vec4 db = vec4(s01.b, s11.b, s10.b, s00.b);
#endif

    vec4 depths = dg;
    // bilateral weights
    //vec4 depths;
    //depths.x = unpack(vec2(dg.x, db.x));
    //depths.y = unpack(vec2(dg.y, db.y));
    //depths.z = unpack(vec2(dg.z, db.z));
    //depths.w = unpack(vec2(dg.w, db.w));

    // bilinear weights
    vec2 f = fract(uv * vec2(textureSize(RT, 0)) - 0.5);
    vec4 b;
    b.x = (1.0 - f.x) * f.y;
    b.y = f.x * f.y;
    b.z = f.x * (1.0 - f.y);
    b.w = (1.0 - f.x) * (1.0 - f.y);

    highp vec4 w = vec4(d, d, d, d) - depths;
    w = max(vec4(MEDIUMP_FLT_MIN, MEDIUMP_FLT_MIN, MEDIUMP_FLT_MIN, MEDIUMP_FLT_MIN), 1.0 - w * w) * b;
    vec4 weights = w / (w.x + w.y + w.z + w.w);
    float z = dot(ao, weights);

    FragColor = vec3(z, z, z);
}
