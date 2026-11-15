#version 450

layout(location = 0) in vec2 inPosition; // ( x, y )
layout(location = 1) in vec4 inColor;    // ( R; G; B; a )

layout(location = 0) out vec4 fragColor;

layout(push_constant) uniform PushConstants {
    vec2 screenResolution;
} pc;

void main() {

    vec2 normalizedPos = (inPosition / pc.screenResolution) * 2.0 - 1.0;    // [ -1, 1 ]
    
    gl_Position = vec4(normalizedPos, 0.0, 1.0);
    fragColor = inColor;
}

// glslc -fshader-stage=vertex ui_vert.glsl -o compiled/ui_vert.spv