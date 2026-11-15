#version 450

layout (location = 0) in vec4 fragColor;
layout (location = 0) out vec4 outColor;

void main(){
    outColor = fragColor;
}

// glslc -fshader-stage=fragment ui_frag.glsl -o compiled/ui_frag.spv