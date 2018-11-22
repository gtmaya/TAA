#version 430

uniform mat4 inverseViewProjectionCURRENT;
uniform mat4 viewProjectionHISTORY;
uniform sampler2D colourRENDER;
uniform sampler2D depthRENDER;
uniform sampler2D colourANTIALIASED;
uniform vec2 windowSize;
uniform vec2 jitter;
uniform float feedback;

layout (location=0) out vec4 FragColour;

in vec2 FragmentUV;

uniform mat3 YCoCGMatrix = mat3(0.25f, 0.5f, -0.25f, 0.5f, 0.f, 0.5f, 0.25f, -0.5f, -0.25f);

uniform mat3 RGBMatrix = mat3(1.f, 1.f, 1.f, 1.f, 0.f, -1.f, -1.f, 1.f, -1.f);

vec3 YCoCg(vec3 _inRGB)
{
  return YCoCGMatrix * _inRGB;
}

vec3 RGB(vec3 _inYCoCG)
{
  return RGBMatrix * _inYCoCG;
}

vec3 sampleRender(vec2 _UV)
{
  return texture(colourRENDER, _UV).rgb;
}

vec3 minSample(vec2 _uvCURRENT, vec2 _pixelDimensions)
{
  vec3 minSamp;
  vec3 samp[5];
  samp[0] = sampleRender(_uvCURRENT);
  samp[1] = sampleRender(_uvCURRENT + vec2(_pixelDimensions.x, 0.f));
  samp[2] = sampleRender(_uvCURRENT - vec2(_pixelDimensions.x, 0.f));
  samp[3] = sampleRender(_uvCURRENT + vec2(0.f, _pixelDimensions.y));
  samp[4] = sampleRender(_uvCURRENT - vec2(0.f, _pixelDimensions.y));

  for (int i = 0; i < 4; i++)
  {
    minSamp = min(samp[i], samp[i + 1]);
  }
  return minSamp;
}

vec3 maxSample(vec2 _uvCURRENT, vec2 _pixelDimensions)
{
  vec3 maxSamp;
  vec3 samp[5];
  samp[0] = sampleRender(_uvCURRENT);
  samp[1] = sampleRender(_uvCURRENT + vec2(_pixelDimensions.x, 0.f));
  samp[2] = sampleRender(_uvCURRENT - vec2(_pixelDimensions.x, 0.f));
  samp[3] = sampleRender(_uvCURRENT + vec2(0.f, _pixelDimensions.y));
  samp[4] = sampleRender(_uvCURRENT - vec2(0.f, _pixelDimensions.y));

  for (int i = 0; i < 4; i++)
  {
    maxSamp = max(samp[i], samp[i + 1]);
  }
  return maxSamp;
}

void main()
{
  bool clamped = false;
  vec2 uvCURRENT = gl_FragCoord.xy / windowSize;
  vec2 pixelDimensions = uvCURRENT / windowSize;
  vec4 colourCURRENT = texture(colourRENDER, uvCURRENT + jitter);
  float depthCURRENT = texture(depthRENDER, uvCURRENT + jitter).r;
  float z = depthCURRENT * 2.f - 1.f;
  vec4 screenSpaceCURRENT = vec4((uvCURRENT) * 2.f - 1.f, z, 1.f);
  vec4 worldSpacePosition = inverseViewProjectionCURRENT * screenSpaceCURRENT;
  worldSpacePosition /= worldSpacePosition.w;

  vec4 screenSpaceHISTORY = viewProjectionHISTORY * worldSpacePosition;
  vec2 uvHISTORY = 0.5 * (screenSpaceHISTORY.xy / screenSpaceHISTORY.w) + 0.5;
  vec4 colourHISTORY = texture(colourANTIALIASED, uvHISTORY);

  vec3 colourMIN = YCoCg(minSample(uvCURRENT + jitter, pixelDimensions));
  vec3 colourMAX = YCoCg(maxSample(uvCURRENT + jitter, pixelDimensions));

  colourHISTORY.xyz = YCoCg(colourHISTORY.rgb);

  if (colourHISTORY.xyz != clamp(colourHISTORY.xyz, colourMIN, colourMAX))
  {
    colourHISTORY.xyz = clamp(colourHISTORY.xyz, colourMIN, colourMAX);
    clamped = true;
  }

  colourHISTORY.rgb = RGB(colourHISTORY.xyz);

  FragColour = feedback * colourCURRENT + (1.f - feedback) * colourHISTORY;
  FragColour = vec4(clamped, 0.f, 0.f, 1.f);
}

