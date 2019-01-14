#version 430

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec2 VertexUV;

uniform mat4 MVP;

out vec2 FragmentUV;

void main()
{
  FragmentUV = VertexUV;
  gl_Position = MVP * vec4(VertexPosition, 1.0);
}

