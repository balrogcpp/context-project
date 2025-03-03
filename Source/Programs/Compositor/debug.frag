// created by Andrey Vasiliev
//? #version 400

uniform sampler2D RT;
uniform vec4 ViewportSize;

out vec4 FragColor;
void main()
{
    vec2 uv = gl_FragCoord.xy * ViewportSize.zw;
    uv.y = 1.0 - uv.y;
    vec3 a = textureLod(RT, uv, 0.0).xyz;
    float z = max(max(a.x, a.y), a.z);

    FragColor = vec4(z, z, z, 1.0);
}
