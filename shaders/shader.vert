#version 450

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 projection;
    mat4 model;
} camera;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 fragColor;

void main(){
    
    gl_Position = camera.projection * camera.view *camera.model * vec4(inPosition, 1.0);

    fragColor = vec3(inPosition.z);
}

// glslc shader.vert -o compiled/vert.spv