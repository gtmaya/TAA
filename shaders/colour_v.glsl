#version 430

uniform mat4 MV;
uniform mat4 MVP;

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexNormal;
layout (location=2) in vec2 TexCoord;

out vec3 LightIntensity;

void main()
{
    gl_Position = MVP * vec4(VertexPosition, 1.0);

    LightIntensity = vec3(VertexPosition);
}
