#version 450

layout(std140, set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 projection;
    mat4 model;
    vec4 colorStart;
    vec4 colorEnd;
    float alpha;
} camera;

layout(location = 0) in float fragHeight;
layout(location = 1) in vec4 explicitColor;
layout(location = 2) in float useExplicitColor;
layout(location = 0) out vec4 outColor;

void main(){
    float normalizedHeight = clamp((fragHeight + 2.0) / 4.0, 0.0, 1.0);
    vec3 finalColor = mix(camera.colorStart.rgb, camera.colorEnd.rgb, normalizedHeight);
    float alpha = camera.alpha;
    if (useExplicitColor > 0.5) {
        finalColor = explicitColor.rgb;
        alpha = explicitColor.a;
    }
    outColor = vec4(finalColor, alpha);
}