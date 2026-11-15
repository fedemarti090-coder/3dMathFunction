#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 0) out vec3 fragColor;

void main(){
    float fov = 2.0f;
    float z = inPosition.z + 2.0f;

    float x_ = fov * inPosition.x / z;
    float y_ = fov * inPosition.y / z;

    gl_Position = vec4(x_, y_, inPosition.z, 1.0f); // cordinate NDC normalizzate

    fragColor = vec3(inPosition.z + 0.5f, 0.5f - inPosition.z, 1.0f);
}

// glslc shader.vert -o compiled/vert.spv