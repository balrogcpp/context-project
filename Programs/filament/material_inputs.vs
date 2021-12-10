struct MaterialVertexInputs {
#ifdef HAS_ATTRIBUTE_COLOR
    vec4 color;
#endif
#ifdef HAS_ATTRIBUTE_UV0
    vec2 uv0;
#endif
#ifdef HAS_ATTRIBUTE_UV1
    vec2 uv1;
#endif
#ifdef VARIABLE_CUSTOM0
    vec4 VARIABLE_CUSTOM0;
#endif
#ifdef VARIABLE_CUSTOM1
    vec4 VARIABLE_CUSTOM1;
#endif
#ifdef VARIABLE_CUSTOM2
    vec4 VARIABLE_CUSTOM2;
#endif
#ifdef VARIABLE_CUSTOM3
    vec4 VARIABLE_CUSTOM3;
#endif
#ifdef HAS_ATTRIBUTE_TANGENTS
    vec3 worldNormal;
#endif
    vec4 worldPosition;
#ifdef MATERIAL_HAS_CLIP_SPACE_TRANSFORM
    mat4 clipSpaceTransform;
#endif
};

// Workaround for a driver bug on ARM Bifrost GPUs. Assigning a structure member
// (directly or inside an expression) to an invariant causes a driver crash.
vec4 getWorldPosition(const MaterialVertexInputs material) {
    return material.worldPosition;
}

#ifdef MATERIAL_HAS_CLIP_SPACE_TRANSFORM
mat4 getClipSpaceTransform(const MaterialVertexInputs material) {
    return material.clipSpaceTransform;
}
#endif

void initMaterialVertex(out MaterialVertexInputs material) {
#ifdef HAS_ATTRIBUTE_COLOR
    // SNiLD: Modified for Ogre mesh_color => colour
    material.color = colour;
#endif
#ifdef HAS_ATTRIBUTE_UV0
    // SNiLD: Modified for Ogre mesh_uv0 => uv0
    #ifdef FLIP_UV_ATTRIBUTE
    material.uv0 = vec2(uv0.x, 1.0 - uv0.y);
    #else
    material.uv0 = uv0;
    #endif
#endif
#ifdef HAS_ATTRIBUTE_UV1
    // SNiLD: Modified for Ogre mesh_uv1 => uv1
    #ifdef FLIP_UV_ATTRIBUTE
    material.uv1 = vec2(uv1.x, 1.0 - uv1.y);
    #else
    material.uv1 = uv1;
    #endif
#endif
#ifdef VARIABLE_CUSTOM0
    material.VARIABLE_CUSTOM0 = vec4(0.0);
#endif
#ifdef VARIABLE_CUSTOM1
    material.VARIABLE_CUSTOM1 = vec4(0.0);
#endif
#ifdef VARIABLE_CUSTOM2
    material.VARIABLE_CUSTOM2 = vec4(0.0);
#endif
#ifdef VARIABLE_CUSTOM3
    material.VARIABLE_CUSTOM3 = vec4(0.0);
#endif
    material.worldPosition = computeWorldPosition();
#ifdef MATERIAL_HAS_CLIP_SPACE_TRANSFORM
    material.clipSpaceTransform = mat4(1.0);
#endif
}
