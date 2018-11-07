#version 430

uniform sampler2D fboTex;
uniform sampler2D fboDepthTex;
uniform sampler2D pastfboTex;
uniform sampler2D pastfboDepthTex;
uniform vec2 windowSize;

layout (location=0) out vec4 FragColour;

in vec2 FragmentUV;

void main()
{
  // Determine the texture coordinate from the window size
  vec2 texpos = gl_FragCoord.xy / windowSize;
  vec4 current = texture(fboTex, texpos);
  vec4 past = texture(pastfboTex, texpos);
  float greyscaleCurrent = (0.3 * current.r) + (0.59 * current.g) + (0.11 * current.b);
  float greyscalePast= (0.3 * past.r) + (0.59 * past.g) + (0.11 * past.b);
  FragColour = vec4(greyscaleCurrent, greyscalePast, 0.f, 1.f);
  FragColour = vec4(current.rgb - past.rgb, 1.f);
}

