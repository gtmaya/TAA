#version 430

uniform sampler2D inputTex;
uniform vec2 windowSize;

layout (location=0) out vec4 FragColour;

in vec2 FragmentUV;

void main()
{
  vec2 uv = gl_FragCoord.xy / windowSize;
  FragColour = texture(inputTex, uv);
}

