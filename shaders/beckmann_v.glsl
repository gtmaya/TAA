#version 430

uniform mat4 MVP;
uniform mat4 MV;
uniform mat3 N;
uniform mat4 MVPNJPrevious;
uniform mat4 MVPNoJitter;

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexNormal;
layout (location=2) in vec2 TexCoord;

out vec3 worldPos;
out vec4 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoord;
out vec3 viewSpacePos;
out vec2 screenSpaceVel;

void main()
{
  fragNormal = normalize(N * VertexNormal);
  worldPos = VertexPosition;
  fragTexCoord = TexCoord;
  gl_Position = MVP * vec4(VertexPosition, 1.f);
  fragPosition = gl_Position;
  viewSpacePos = (MV * vec4(VertexPosition, 1.f)).xyz;
  vec3 screenSpaceCURRENT = (MVPNoJitter * vec4(VertexPosition, 1.f)).xyz;
  vec3 screenSpaceHISTORY = (MVPNJPrevious * vec4(VertexPosition, 1.f)).xyz;
  vec3 diff = screenSpaceHISTORY - screenSpaceCURRENT;
  screenSpaceVel = diff.xy;
}
