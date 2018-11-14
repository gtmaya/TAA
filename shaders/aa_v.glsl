#version 430

// The vertex position attribute
layout (location=0) in vec3 VertexPosition;

// The UV position
layout (location=1) in vec2 VertexUV;

// We need an MVP because the plane needs to be rotated
uniform mat4 MVP;

out vec2 FragmentUV;

void main() {
    // Pass the fragment UV coordinates
    FragmentUV = VertexUV;

    // Set the position of the current vertex
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}

