// lighting.vert
#version 450

layout(location = 0) out vec2 fragUV;

// Efficient full-screen quad using a single triangle
const vec2 positions[3] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 3.0, -1.0),
    vec2(-1.0,  3.0)
);

void main() {
    vec2 position = positions[gl_VertexIndex];
    gl_Position = vec4(position, 0.0, 1.0);
    
    // Transform to UV space [0,1]
    fragUV = (position * 0.5 + 0.5);
}