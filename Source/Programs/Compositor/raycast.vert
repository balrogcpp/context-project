// created by Andrey Vasiliev
//? #version 400

in highp vec3 normal;
out highp vec3 vRay;
void main()
{
    highp vec4 positions[4] = vec4[4](
		vec4(-1.0, -1.0, 0.0, 1.0),
		vec4(1.0, -1.0, 0.0, 1.0),
		vec4(-1.0, 1.0, 0.0, 1.0),
		vec4(1.0, 1.0, 0.0, 1.0));

    gl_Position = positions[gl_VertexID];
    vRay = normal.xyz;
}
