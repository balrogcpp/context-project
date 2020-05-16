#version 120
//#extension GL_ARB_draw_buffers : enable

varying    vec3 oViewPos;
//varying    vec3 oNormal;
varying     vec2 vUV;

uniform float cNearClipDistance;
uniform float cFarClipDistance;// !!! might be 0 for infinite view projection.
uniform sampler2D baseColor;

void main()
{
  vec4 color = texture2D(baseColor, vUV);
  if (color.a == 0.0) {
    discard;
  }

  float distance = length(oViewPos);
  if (distance > 1000.0) {
    discard;
  }

  float clipDistance = cFarClipDistance - cNearClipDistance;
  gl_FragColor = vec4((distance - cNearClipDistance) / clipDistance, 0.0, 0.0, 0.0);// view space position
}