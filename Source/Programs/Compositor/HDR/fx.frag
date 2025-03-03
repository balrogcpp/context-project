// created by Andrey Vasiliev
//? #version 400

uniform sampler2D RT;
uniform float Time;

// Applies a vignette effect
// color: source color to apply the vignette on top of
// uv: viewport coordinates
// vignette: pre-computed parameters midPoint, radius, aspect and feather
// vignetteColor: color of the vignette effect
vec3 vignette(const vec3 color, const highp vec2 uv, const vec4 vignette, const vec4 vignetteColor) {
    float midPoint = vignette.x;
    float radius = vignette.y;
    float aspect = vignette.z;
    float feather = vignette.w;

    vec2 distance = abs(uv - 0.5) * midPoint;
    distance.x *= aspect;
    distance = pow(clamp(distance, 0.0, 1.0), vec2(radius));

    float amount = pow(clamp(1.0 - dot(distance, distance), 0.0, 1.0), feather * 5.0);
    return color * mix(vignetteColor.rgb, vec3(1.0), amount);
}

vec3 grain(const vec3 color, const highp vec2 uv) {
    // Generate random noise
    float noise = (fract(sin(dot(uv * Time, vec2(12.9898, 78.233))) * 43758.5453) - 0.5) * 2.0;

    // Add noise to the original color
    return color + noise * 0.04;
}

out vec3 FragColor;
void main()
{
    vec2 uv = gl_FragCoord.xy / vec2(textureSize(RT, 0));
    vec3 color = texelFetch(RT, ivec2(gl_FragCoord.xy), 0).rgb;
    color = grain(color, uv);
    //color = vignette(color, uv, vec4(1, 1, 1, 1), vec4(0.0, 0.0, 0.0, 1.0));
    FragColor.rgb = color;
}
