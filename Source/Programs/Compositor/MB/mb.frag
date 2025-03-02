// created by Andrey Vasiliev
//? #version 400

uniform mediump sampler2D RT;
uniform mediump sampler2D DepthTex;
uniform vec4 ZBufferParams;
uniform mat4 InvViewMatrix;
uniform mat4 WorldViewProjMatrix;
uniform mat4 ViewProjPrev;
uniform float FrameTime;
uniform float ClampDistance;

#define MAX_SAMPLES 8

float Linear01Depth(const highp float z)
{
    return 1.0 / (z * ZBufferParams.x + ZBufferParams.y);
}

vec4 mulMat4x4Half3(const mat4 m, const vec3 v)
{
    return v.x * m[0] + (v.y * m[1] + (v.z * m[2] + m[3]));
}

vec3 GetCameraVec(const vec2 uv)
{
    // Returns the vector from camera to the specified position on the camera plane (uv argument), located one unit away from the camera
    // This vector is not normalized.
    // The nice thing about this setup is that the returned vector from this function can be simply multiplied with the linear depth to get pixel's position relative to camera position.
    // This particular function does not account for camera rotation or position or FOV at all (since we don't need it for AO)
    // TODO: AO is dependent on FOV, this function is not!
    // The outcome of using this simplified function is that the effective AO range is larger when using larger FOV
    // Use something more accurate to get proper FOV-independent world-space range, however you will likely also have to adjust the SSAO constants below
    vec2 tsize = textureSize(DepthTex, 0);
    float aspect = tsize.y / tsize.x;
    return vec3(-(-uv.y * aspect + aspect),- (uv.x * 2.0 - 1.0), -1.0);
}


//in highp vec3 vRay;
out vec3 FragColor;
void main()
{
    vec2 uv = vec2(gl_FragCoord.xy) / vec2(textureSize(RT, 0));

    vec3 color = textureLod(RT, uv, 0.0).rgb;
    float depth = Linear01Depth(textureLod(DepthTex, vec2(uv.x, 1.0 - uv.y), 0.0).x);

    vec3 ray = GetCameraVec(uv) * ClampDistance;

    //vec3 viewPos = vRay * depth;
    vec3 viewPos = ray * depth;
    vec4 worldPos = mulMat4x4Half3(InvViewMatrix, viewPos.xyz);
    worldPos.xyz /= worldPos.w;

    vec4 nuv = mulMat4x4Half3(ViewProjPrev, worldPos.xyz);
    nuv.xy /= nuv.w;

    vec4 olduv = mulMat4x4Half3(WorldViewProjMatrix, worldPos.xyz);
    olduv.xy /= olduv.w;

    vec2 velocity = (nuv.xy - olduv.xy);

    velocity *= (16.6666666667 /  FrameTime);
    float speed = length(velocity * vec2(textureSize(RT, 0)));
    float nSamples = ceil(clamp(speed, 1.0, float(MAX_SAMPLES)));
    float invSamples = 1.0 / nSamples;
    float counter = 1.0;

    for (int i = 1; i < MAX_SAMPLES; ++i) {
        if (int(nSamples) <= i) break;

        vec2 offset = (float(i) * invSamples - 0.5) * velocity;
        color += textureLod(RT, uv + offset, 0.0).rgb;
        counter += 1.0;
    }

    color /= counter;

    FragColor = color;
}
