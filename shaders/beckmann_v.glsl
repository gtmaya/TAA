/****************************************************************************************************************
/__/\\\\\\\\\\\\\\\_____/\\\\\\\\\________/\\\\\\\\\____________/                                               |
/__\///////\\\/////____/\\\\\\\\\\\\\____/\\\\\\\\\\\\\_________/   Callum James Glover                         |
/_________\/\\\________/\\\/////////\\\__/\\\/////////\\\_______/   NCCA, Bournemouth University                |
/__________\/\\\_______\/\\\_______\/\\\_\/\\\_______\/\\\______/   s4907224@bournemouth.ac.uk                  |
/___________\/\\\_______\/\\\\\\\\\\\\\\\_\/\\\\\\\\\\\\\\\_____/   callum@glovefx.com                          |
/____________\/\\\_______\/\\\/////////\\\_\/\\\/////////\\\____/   07946 750075                                |
/_____________\/\\\_______\/\\\_______\/\\\_\/\\\_______\/\\\___/   Level 6 Computing for Animation Project     |
/______________\/\\\_______\/\\\_______\/\\\_\/\\\_______\/\\\__/   https://github.com/NCCA/CA1-2018-s4907224   |
/_______________\///________\///________\///__\///________\///__/                                               |
****************************************************************************************************************/
//---------------------------------------------------------------------------------------------------------------
/// @file beckmann_v.glsl
/// @brief This shader is partially modified from a previous project, available at
/// https://github.com/s4907224/s4907224_GLSL
//---------------------------------------------------------------------------------------------------------------
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
//Velocity output
out vec2 screenSpaceVel;

void main()
{
  fragNormal = normalize(N * VertexNormal);
  worldPos = VertexPosition;
  fragTexCoord = TexCoord;
  gl_Position = MVP * vec4(VertexPosition, 1.f);
  fragPosition = gl_Position;
  viewSpacePos = (MV * vec4(VertexPosition, 1.f)).xyz;

  //Velocity buffer things:
  //Find the unjittered position of the current vertex in screen space for *this* frame
  vec3 screenSpaceCURRENT = (MVPNoJitter * vec4(VertexPosition, 1.f)).xyz;
  //Find the unjittered position of the current vertex in screen space for the previous frame
  vec3 screenSpaceHISTORY = (MVPNJPrevious * vec4(VertexPosition, 1.f)).xyz;
  //Find a vector that is essentially the negative velocity, this makes more sense later as we can simply add the velocity
  //buffer to the jitter offset as opposed to negating it.
  screenSpaceVel = (screenSpaceHISTORY - screenSpaceCURRENT).xy;
}
