#version 430

uniform mat4 MVP;
uniform mat4 MV;
uniform mat3 N;
uniform vec2 jitter;


layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexNormal;
layout (location=2) in vec2 TexCoord;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoord;
out vec4 testOut;

void main()
{
  fragNormal = normalize(N * VertexNormal);
  fragPosition = vec3(MV * vec4(VertexPosition, 1.0) );
  fragTexCoord = TexCoord;
  gl_Position = MVP * vec4(VertexPosition,1.0);
  testOut = MVP * vec4(VertexPosition, 1.f);
  gl_Position.xy += jitter.xy;
}
