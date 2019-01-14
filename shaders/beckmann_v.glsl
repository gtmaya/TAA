#version 430

uniform mat4 MVP;
uniform mat4 MV;
uniform mat3 N;
uniform mat4 M;

uniform vec3 objectCentre;
uniform vec3 linearVel;
uniform vec3 angularVel;

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexNormal;
layout (location=2) in vec2 TexCoord;

out vec3 worldPos;
out vec4 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoord;
out vec3 viewSpacePos;
out vec3 worldSpaceVel;

void main()
{
  fragNormal = normalize(N * VertexNormal);
  worldPos = VertexPosition;
  fragTexCoord = TexCoord;
  gl_Position = MVP * vec4(VertexPosition, 1.f);
  fragPosition = gl_Position;
  viewSpacePos = (MV * vec4(VertexPosition, 1.f)).xyz;
  vec3 centreToVertex = (M * vec4(VertexPosition, 1.f) - M * vec4(objectCentre, 1.f)).xyz;

  vec3 radVecYZ = vec3(0.f, centreToVertex.y, centreToVertex.z);
  float vYZ = length(radVecYZ) * angularVel[0];
  vec3 tangentYZ = cross(radVecYZ, vec3(1.f, 0.f, 0.f));

  vec3 radVecXZ = vec3(centreToVertex.x, 0.f, centreToVertex.z);
  float vXZ = length(radVecXZ) * angularVel[1];
  vec3 tangentXZ = cross(radVecXZ, vec3(0.f, 1.f, 0.f));

  vec3 radVecXY = vec3(centreToVertex.x, centreToVertex.y, 0.f);
  float vXY = length(radVecXY) * angularVel[2];
  vec3 tangentXY = cross(radVecYZ, vec3(0.f, 0.f, 1.f));

  vec3 ang2Lin = vYZ * tangentYZ + vXZ * tangentXZ + vXY * tangentXY;

  worldSpaceVel = ang2Lin + linearVel;
}
