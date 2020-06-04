#ifndef GL_ES
#version 120
#else
#version 100
#endif

uniform vec4 shadowColor;

void main() {
    gl_FragColor = shadowColor;
}
