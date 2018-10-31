#version 430 core

layout (location=0) out vec4 FragColour;

in vec3 worldSpace;

uniform samplerCube envMap;

void main ()
{
  vec3 lookup = normalize(worldSpace);
  lookup.y *= -1;
  lookup.z *= -1;
  FragColour = texture(envMap, lookup);
}

