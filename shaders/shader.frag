#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

void main(){

    float normalizedz = (fragColor.r + 2.0) / 4.0;
    normalizedz = clamp(normalizedz, 0.0, 1.0);

    vec3 colorBlue = vec3(0.0, 0.4, 1.0);
    vec3 colorRed = vec3(1.0, 0.1, 0.2);

    vec3 finalColor = mix(colorBlue, colorRed, normalizedz);

    outColor = vec4(finalColor, 1.0f);
}

// glslc shader.frag -o compiled/frag.spv