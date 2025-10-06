#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D lightingResult;

void main() {
    // Direct sampling of lighting result
    outColor = texture(lightingResult, fragTexCoord);
}