#version 430

layout (location = 1) out vec3 velocity;

in vec2 screenSpaceVel;

void main()
{
  velocity = vec3(screenSpaceVel, 0.f);
}
