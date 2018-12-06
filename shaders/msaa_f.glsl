#version 430

uniform mat4 inverseViewProjectionCURRENT;
uniform mat4 viewProjectionHISTORY;
uniform sampler2D colourRENDER;
uniform sampler2D depthRENDER;
uniform sampler2D colourANTIALIASED;
uniform vec2 windowSize;
uniform vec2 pixelSize;
uniform vec2 jitter;
uniform float feedback;

layout (location=0) out vec4 FragColour;

in vec2 FragmentUV;

uniform mat3 YCoCGMatrix = mat3(0.25f, 0.5f, -0.25f, 0.5f, 0.f, 0.5f, 0.25f, -0.5f, -0.25f);

uniform mat3 RGBMatrix = mat3(1.f, 1.f, 1.f, 1.f, 0.f, -1.f, -1.f, 1.f, -1.f);

bool clamped = false;

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

vec3 minSample(vec2 _uvCURRENT)
{
  vec3 minSamp = vec3(1.f, 1.f, 1.f);
  vec3 samp[9];
  samp[0] = sampleRender(_uvCURRENT);
  samp[1] = sampleRender(_uvCURRENT + vec2( pixelSize.x, 0.f));
  samp[2] = sampleRender(_uvCURRENT + vec2(-pixelSize.x, 0.f));
  samp[3] = sampleRender(_uvCURRENT + vec2(0.f,  pixelSize.y));
  samp[4] = sampleRender(_uvCURRENT + vec2(0.f, -pixelSize.y));
  samp[5] = sampleRender(_uvCURRENT + vec2( pixelSize.x,  pixelSize.y));
  samp[6] = sampleRender(_uvCURRENT + vec2(-pixelSize.x,  pixelSize.y));
  samp[7] = sampleRender(_uvCURRENT + vec2( pixelSize.x, -pixelSize.y));
  samp[8] = sampleRender(_uvCURRENT + vec2(-pixelSize.x, -pixelSize.y));

  for (int i = 0; i < 9; i++)
  {
    minSamp = min(samp[i], minSamp);
  }
  return minSamp;
}

vec3 maxSample(vec2 _uvCURRENT)
{
  vec3 maxSamp = vec3(0.f, 0.f, 0.f);
  vec3 samp[9];
  samp[0] = sampleRender(_uvCURRENT);
  samp[1] = sampleRender(_uvCURRENT + vec2( pixelSize.x, 0.f));
  samp[2] = sampleRender(_uvCURRENT + vec2(-pixelSize.x, 0.f));
  samp[3] = sampleRender(_uvCURRENT + vec2(0.f,  pixelSize.y));
  samp[4] = sampleRender(_uvCURRENT + vec2(0.f, -pixelSize.y));
  samp[5] = sampleRender(_uvCURRENT + vec2( pixelSize.x,  pixelSize.y));
  samp[6] = sampleRender(_uvCURRENT + vec2(-pixelSize.x,  pixelSize.y));
  samp[7] = sampleRender(_uvCURRENT + vec2( pixelSize.x, -pixelSize.y));
  samp[8] = sampleRender(_uvCURRENT + vec2(-pixelSize.x, -pixelSize.y));

  for (int i = 0; i < 9; i++)
  {
    maxSamp = max(samp[i], maxSamp);
  }
  return maxSamp;
}

vec3 clipNeighbourhood(vec3 _colourSample, vec2 _uvCURRENT)
{
  vec3 colourMIN = YCoCg(minSample(_uvCURRENT));
  vec3 colourMAX = YCoCg(maxSample(_uvCURRENT));

  vec3 YCoCgSample = YCoCg(_colourSample.rgb);

  vec3 aabbCentre = 0.5f * (colourMAX + colourMIN);
  vec3 aabbWidth = 0.5f * (colourMAX - colourMIN);
  vec3 centreToSample = YCoCgSample - aabbCentre;
  vec3 ctsUnitSpace = centreToSample / aabbWidth;
  float maximumDimension = max(abs(ctsUnitSpace.x), max(abs(ctsUnitSpace.y), abs(ctsUnitSpace.z)));
  if (maximumDimension > 1.f)
  {
    vec3 clippedColour = aabbCentre + (centreToSample / maximumDimension);
    clamped = true;
    return RGB(clippedColour);
  }
  else {return RGB(YCoCgSample);}
}

void main()
{
  vec2 uvCURRENT = gl_FragCoord.xy / windowSize;
  vec4 colourCURRENT = texture(colourRENDER, uvCURRENT - jitter);
  float depthCURRENT = texture(depthRENDER, uvCURRENT - jitter).r;

  float z = depthCURRENT * 2.f - 1.f;
  vec4 screenSpaceCURRENT = vec4((uvCURRENT) * 2.f - 1.f, z, 1.f);
  vec4 worldSpacePosition = inverseViewProjectionCURRENT * screenSpaceCURRENT;

  worldSpacePosition /= worldSpacePosition.w;

  vec4 screenSpaceHISTORY = viewProjectionHISTORY * worldSpacePosition;
  vec2 uvHISTORY = 0.5 * (screenSpaceHISTORY.xy / screenSpaceHISTORY.w) + 0.5;
  vec4 colourHISTORY = texture(colourANTIALIASED, uvHISTORY);

  colourHISTORY.rgb = clipNeighbourhood(colourHISTORY.rgb, uvCURRENT);

  FragColour = feedback * colourCURRENT + (1.f - feedback) * colourHISTORY;
}

