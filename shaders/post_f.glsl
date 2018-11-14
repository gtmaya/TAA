#version 430

//uniform mat4 projectionCURRENT;
uniform mat4 inverseViewProjectionCURRENT;
uniform mat4 inverseViewCURRENT;
//uniform mat4 viewPREVIOUS;
uniform mat4 viewProjectionPREVIOUS;

uniform sampler2D fboTex;
uniform sampler2D fboDepthTex;
uniform sampler2D pastfboTex;
uniform sampler2D pastfboDepthTex;
uniform vec2 windowSize;
uniform vec3 jitter;

layout (location=0) out vec4 FragColour;

in vec2 FragmentUV;

void main()
{
  vec2 uvCURRENT = gl_FragCoord.xy / windowSize;
  vec4 colourCURRENT = texture(fboTex, uvCURRENT - jitter.xy);
  float depthCURRENT = texture(fboDepthTex, uvCURRENT).r;
  float z = depthCURRENT * 2.f - 1.f;
  vec4 clipSpacePosition = vec4(uvCURRENT * 2.f - 1.f, z, 1.f);
  vec4 worldSpacePosition = inverseViewProjectionCURRENT * clipSpacePosition;
  worldSpacePosition /= worldSpacePosition.w;

  vec4 test = viewProjectionPREVIOUS * worldSpacePosition;
  vec2 uvPREVIOUS = 0.5 * ( test.xy / test.w ) + 0.5;
  vec4 colourPREVIOUS = texture(pastfboTex, uvPREVIOUS);

  FragColour = 0.5 * colourCURRENT + 0.5 * colourPREVIOUS;
  FragColour = colourCURRENT;
}

