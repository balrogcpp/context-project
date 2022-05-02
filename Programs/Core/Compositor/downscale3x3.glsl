// This source file is part of Glue Engine. Created by Andrey Vasiliev

//----------------------------------------------------------------------------------------------------------------------
vec3 Downscale3x3(const sampler2D sampler, const vec2 uv, const vec2 tsize) {
    const vec3 weights0 = vec3(0.0625); // 1/16
    const vec3 weights1 = vec3(0.125); // 2/16
    const vec3 weights2 = vec3(0.0625); // 1/16
    const vec3 weights3 = vec3(0.125); // 2/16
    const vec3 weights4 = vec3(0.25); // 4/16
    const vec3 weights5 = vec3(0.125); // 2/16
    const vec3 weights6 = vec3(0.0625); // 1/16
    const vec3 weights7 = vec3(0.125); // 2/16
    const vec3 weights8 = vec3(0.0625); // 1/16

    const vec2 offsets0 = vec2(-1.0, -1.0);
    const vec2 offsets1 = vec2(0.0, -1.0);
    const vec2 offsets2 = vec2(1.0, -1.0);
    const vec2 offsets3 = vec2(-1.0, 0.0);
    const vec2 offsets4 = vec2(0.0, 0.0);
    const vec2 offsets5 = vec2(1.0, 0.0);
    const vec2 offsets6 = vec2(-1.0, 1.0);
    const vec2 offsets7 = vec2(0.0,  1.0);
    const vec2 offsets8 = vec2(1.0, 1.0);

    vec3 color = vec3(0.0);

    color += weights0 * texture2D(sampler, uv + offsets0 * tsize).rgb;
    color += weights1 * texture2D(sampler, uv + offsets1 * tsize).rgb;
    color += weights2 * texture2D(sampler, uv + offsets2 * tsize).rgb;
    color += weights3 * texture2D(sampler, uv + offsets3 * tsize).rgb;
    color += weights4 * texture2D(sampler, uv + offsets4 * tsize).rgb;
    color += weights5 * texture2D(sampler, uv + offsets5 * tsize).rgb;
    color += weights6 * texture2D(sampler, uv + offsets6 * tsize).rgb;
    color += weights7 * texture2D(sampler, uv + offsets7 * tsize).rgb;
    color += weights8 * texture2D(sampler, uv + offsets8 * tsize).rgb;

    return color;
}
