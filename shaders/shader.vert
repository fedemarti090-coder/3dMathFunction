#version 450

layout(std140, set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 projection;
    mat4 model;
    vec4 colorStart;
    vec4 colorEnd;
    float alpha;
} camera;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inExplicitColor;
layout(location = 2) in float inUseExplicitColor;

layout(location = 0) out float fragHeight;
layout(location = 1) out vec4 explicitColor;
layout(location = 2) out float useExplicitColor;

void main(){
    gl_Position = camera.projection * camera.view * camera.model * vec4(inPosition, 1.0);
    fragHeight = inPosition.z;
    explicitColor = inExplicitColor;
    useExplicitColor = inUseExplicitColor;
}

// glslc shader.vert -o compiled/vert.spv