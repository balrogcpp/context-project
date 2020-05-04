#version 120
#extension GL_ARB_draw_buffers : enable

varying    vec3 oViewPos;
varying    vec3 oNormal;
varying     vec2 vUV;

uniform float cNearClipDistance;
uniform float cFarClipDistance;// !!! might be 0 for infinite view projection.
uniform sampler2D baseColor;

void main()
{
    if (texture2D(baseColor, vUV).a == 0.0)
        discard;

    float distance = length(oViewPos);

    if (distance > 1000) {
        discard;
    }

    float clipDistance = cFarClipDistance - cNearClipDistance;
    gl_FragData[0] = vec4(normalize(oNormal).xyz, (distance - cNearClipDistance) / clipDistance);// normal + linear depth [0, 1]
    gl_FragData[1] = vec4(oViewPos, 0.0);// view space position
}