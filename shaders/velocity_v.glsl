#version 430

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 M;

uniform vec3 objectCentre;
uniform vec3 linearVel;
uniform vec3 angularVel;

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexNormal;
layout (location=2) in vec2 TexCoord;

out vec2 screenSpaceVel;

void main()
{
  gl_Position = MVP * vec4(VertexPosition, 1.f);
  vec3 viewSpacePos = (MV * vec4(VertexPosition, 1.f)).xyz;
  vec3 centreToVertex = (M * vec4(VertexPosition, 1.f) - M * vec4(objectCentre, 1.f)).xyz;

  vec3 radVecYZ = vec3(0.f, centreToVertex.y, centreToVertex.z);
  float vYZ = length(radVecYZ) * angularVel[0];
  vec3 tangentYZ = normalize(cross(radVecYZ, vec3(1.f, 0.f, 0.f)));

  vec3 radVecXZ = vec3(centreToVertex.x, 0.f, centreToVertex.z);
  float vXZ = length(radVecXZ) * angularVel[1];
  vec3 tangentXZ = normalize(cross(radVecXZ, vec3(0.f, 1.f, 0.f)));

  vec3 radVecXY = vec3(centreToVertex.x, centreToVertex.y, 0.f);
  float vXY = length(radVecXY) * angularVel[2];
  vec3 tangentXY = normalize(cross(radVecYZ, vec3(0.f, 0.f, 1.f)));

  vec3 ang2Lin = vYZ * tangentYZ + vXZ * tangentXZ + vXY * tangentXY;

  vec3 worldSpaceVel = ang2Lin + linearVel;

  vec3 crossDepthVel = cross(worldSpaceVel, viewSpacePos);
  screenSpaceVel = crossDepthVel.xy;
}
