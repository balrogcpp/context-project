// Directional light extrude - FINITE
#ifndef GL_ES
#version 120
#else
#version 100
#endif

in vec4 uv0;
in vec4 position;

uniform mat4 worldviewproj_matrix;
uniform vec4 light_position_object_space; // homogenous, object space
uniform float shadow_extrusion_distance;  // how far to extrude

void main()
{
    // Extrusion in object space
    // Vertex unmodified if w==1, extruded if w==0
    vec3 extrusionDir = - light_position_object_space.xyz;
    extrusionDir = normalize(extrusionDir);

    vec4 newpos = vec4(position.xyz +
        ((1.0 - uv0.x) * shadow_extrusion_distance * extrusionDir), 1.0);

    gl_Position = worldviewproj_matrix * newpos;
}