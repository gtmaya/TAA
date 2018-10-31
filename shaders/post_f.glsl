#version 430

uniform sampler2D fboTex;
uniform sampler2D fboDepthTex;
uniform vec2 windowSize;

layout (location=0) out vec4 FragColour;

in vec2 FragmentUV;

void main()
{
  // Determine the texture coordinate from the window size
  vec2 texpos = gl_FragCoord.xy / windowSize;
  FragColour = texture(fboTex, texpos);
}

