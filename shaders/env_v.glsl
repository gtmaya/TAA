#version 430

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec2 TexCoord;
layout (location=2) in vec3 VertexNormal;

uniform mat4 MV;
uniform mat4 MVP;

out vec3 worldSpace;

void main()
{
  worldSpace = VertexPosition;
  gl_Position = MVP * vec4(VertexPosition,1.f);
}





