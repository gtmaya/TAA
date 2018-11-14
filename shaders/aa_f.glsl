#version 430

uniform mat4 inverseViewProjectionCURRENT;
uniform mat4 viewProjectionPREVIOUS;
uniform sampler2D colourRENDER;
uniform sampler2D depthRENDER;
uniform sampler2D colourANTIALIASED;
uniform vec2 windowSize;
uniform vec3 jitter;

layout (location=0) out vec4 FragColour;

in vec2 FragmentUV;

void main()
{
  vec2 uvCURRENT = gl_FragCoord.xy / windowSize;
  vec4 colourCURRENT = texture(colourRENDER, uvCURRENT - jitter.xy);
  float depthCURRENT = texture(depthRENDER, uvCURRENT).r;
  float z = depthCURRENT * 2.f - 1.f;
  vec4 clipSpacePosition = vec4(uvCURRENT * 2.f - 1.f, z, 1.f);
  vec4 worldSpacePosition = inverseViewProjectionCURRENT * clipSpacePosition;
  worldSpacePosition /= worldSpacePosition.w;

  vec4 test = viewProjectionPREVIOUS * worldSpacePosition;
  vec2 uvPREVIOUS = 0.5 * ( test.xy / test.w ) + 0.5;
  vec4 colourHISTORY = texture(colourANTIALIASED, uvCURRENT);

  FragColour = 0.5 * colourCURRENT + 0.5 * colourHISTORY;
  FragColour = colourHISTORY;
}

