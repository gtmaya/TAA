#version 430

uniform mat4 inverseViewProjectionCURRENT;
uniform mat4 VPCURRENT;
uniform mat4 viewProjectionHISTORY;
uniform sampler2D velocityBUF;
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

bool clipped = false;
float clipDist = 0.f;

vec3 YCoCg(vec3 _inRGB)
{
  return _inRGB;
  return YCoCGMatrix * _inRGB;
}

vec3 RGB(vec3 _inYCoCG)
{
  return _inYCoCG;
  return RGBMatrix * _inYCoCG;
}

vec3 sampleRenderYCoCg(vec2 _UV)
{
  return YCoCg(texture(colourRENDER, _UV).rgb);
}

vec3 minSample3x3(vec2 _uvCURRENT)
{
  vec3 minSamp = vec3(1.f, 1.f, 1.f);
  vec3 samp[9];
  samp[0] = sampleRenderYCoCg(_uvCURRENT);
  samp[1] = sampleRenderYCoCg(_uvCURRENT + vec2( pixelSize.x, 0.f));
  samp[2] = sampleRenderYCoCg(_uvCURRENT + vec2(-pixelSize.x, 0.f));
  samp[3] = sampleRenderYCoCg(_uvCURRENT + vec2(0.f,  pixelSize.y));
  samp[4] = sampleRenderYCoCg(_uvCURRENT + vec2(0.f, -pixelSize.y));
  samp[5] = sampleRenderYCoCg(_uvCURRENT + vec2( pixelSize.x,  pixelSize.y));
  samp[6] = sampleRenderYCoCg(_uvCURRENT + vec2(-pixelSize.x,  pixelSize.y));
  samp[7] = sampleRenderYCoCg(_uvCURRENT + vec2( pixelSize.x, -pixelSize.y));
  samp[8] = sampleRenderYCoCg(_uvCURRENT + vec2(-pixelSize.x, -pixelSize.y));

  for (int i = 0; i < 9; i++)
  {
    minSamp = min(samp[i], minSamp);
  }
  return minSamp;
}

vec3 minSampleDirectNeighbours(vec2 _uvCURRENT)
{
  vec3 minSamp = vec3(1.f, 1.f, 1.f);
  vec3 samp[5];
  samp[0] = sampleRenderYCoCg(_uvCURRENT);
  samp[1] = sampleRenderYCoCg(_uvCURRENT + vec2( pixelSize.x, 0.f));
  samp[2] = sampleRenderYCoCg(_uvCURRENT + vec2(-pixelSize.x, 0.f));
  samp[3] = sampleRenderYCoCg(_uvCURRENT + vec2(0.f,  pixelSize.y));
  samp[4] = sampleRenderYCoCg(_uvCURRENT + vec2(0.f, -pixelSize.y));

  for (int i = 0; i < 5; i++)
  {
    minSamp = min(samp[i], minSamp);
  }
  return minSamp;
}

vec3 maxSample3x3(vec2 _uvCURRENT)
{
  vec3 maxSamp = vec3(0.f, 0.f, 0.f);
  vec3 samp[9];
  samp[0] = sampleRenderYCoCg(_uvCURRENT);
  samp[1] = sampleRenderYCoCg(_uvCURRENT + vec2( pixelSize.x, 0.f));
  samp[2] = sampleRenderYCoCg(_uvCURRENT + vec2(-pixelSize.x, 0.f));
  samp[3] = sampleRenderYCoCg(_uvCURRENT + vec2(0.f,  pixelSize.y));
  samp[4] = sampleRenderYCoCg(_uvCURRENT + vec2(0.f, -pixelSize.y));
  samp[5] = sampleRenderYCoCg(_uvCURRENT + vec2( pixelSize.x,  pixelSize.y));
  samp[6] = sampleRenderYCoCg(_uvCURRENT + vec2(-pixelSize.x,  pixelSize.y));
  samp[7] = sampleRenderYCoCg(_uvCURRENT + vec2( pixelSize.x, -pixelSize.y));
  samp[8] = sampleRenderYCoCg(_uvCURRENT + vec2(-pixelSize.x, -pixelSize.y));

  for (int i = 0; i < 9; i++)
  {
    maxSamp = max(samp[i], maxSamp);
  }
  return maxSamp;
}

vec3 maxSampleDirectNeighbours(vec2 _uvCURRENT)
{
  vec3 maxSamp = vec3(0.f, 0.f, 0.f);
  vec3 samp[5];
  samp[0] = sampleRenderYCoCg(_uvCURRENT);
  samp[1] = sampleRenderYCoCg(_uvCURRENT + vec2( pixelSize.x, 0.f));
  samp[2] = sampleRenderYCoCg(_uvCURRENT + vec2(-pixelSize.x, 0.f));
  samp[3] = sampleRenderYCoCg(_uvCURRENT + vec2(0.f,  pixelSize.y));
  samp[4] = sampleRenderYCoCg(_uvCURRENT + vec2(0.f, -pixelSize.y));

  for (int i = 0; i < 5; i++)
  {
    maxSamp = max(samp[i], maxSamp);
  }
  return maxSamp;
}

vec3 clipNeighbourhood(vec3 _colourSample, vec2 _uvCURRENT)
{
  vec3 colourMIN = mix(minSample3x3(_uvCURRENT), minSampleDirectNeighbours(_uvCURRENT), 0.5f);
  vec3 colourMAX = mix(maxSample3x3(_uvCURRENT), maxSampleDirectNeighbours(_uvCURRENT), 0.5f);

  vec3 YCoCgSample = YCoCg(_colourSample.rgb);

  vec3 aabbCentre = 0.5f * (colourMAX + colourMIN);
  vec3 aabbWidth = 0.5f * (colourMAX - colourMIN);
  vec3 centreToSample = YCoCgSample - aabbCentre;
  vec3 ctsUnitSpace = centreToSample / aabbWidth;
  float maximumDimension = max(abs(ctsUnitSpace.x), max(abs(ctsUnitSpace.y), abs(ctsUnitSpace.z)));
  if (maximumDimension > 1.f)
  {
    vec3 clippedColour = aabbCentre + (centreToSample / maximumDimension);
    clipped = true;
    clipDist = maximumDimension - 1.f;
    return RGB(clippedColour);
  }
  else {return RGB(YCoCgSample);}
}

void main()
{
  vec2 uvCURRENT = gl_FragCoord.xy / windowSize;

  //Get current frame data
  vec4 colourCURRENT = texture(colourRENDER, uvCURRENT - jitter);
  float depthCURRENT = texture(depthRENDER, uvCURRENT - jitter).r;

  //Convert current screenspace to world space
  float z = depthCURRENT * 2.0 - 1.0;
  vec4 screenSpaceCURRENT = vec4((uvCURRENT) * 2.f - 1.f, z, 1.f);
  vec4 worldSpacePosition = inverseViewProjectionCURRENT * screenSpaceCURRENT;
  worldSpacePosition /= worldSpacePosition.w;

  //Convert this into previous UV coords.
  vec4 screenSpaceHISTORY = viewProjectionHISTORY * worldSpacePosition;
  vec2 uvHISTORY = 0.5 * (screenSpaceHISTORY.xy / screenSpaceHISTORY.w) + 0.5;

  vec2 vel = uvCURRENT - uvHISTORY;
  vel += texture(velocityBUF, uvCURRENT - jitter).rg * pixelSize;
  uvHISTORY = uvCURRENT - vel;

  //Get previous frame colour
  vec4 colourHISTORY = texture(colourANTIALIASED, vec2(uvHISTORY));

  //Clip it
  vec3 colourHISTORYCLIPPED = clipNeighbourhood(colourHISTORY.rgb, uvCURRENT);

  if (colourHISTORY.a == 0.f) {FragColour.a = float(clipped);} //If there's nothing, store the clipped flag (could still be nothing)
  else {FragColour.a = mix(colourHISTORY.a, float(clipped), feedback);} //If there is something, blend the previous clipped value with the current one (using same feedback as rest of AA)

  float clipBlendFactor = FragColour.a;
  vec3 colourHISTORYCLIPPEDBLEND = mix(colourHISTORY.rgb, colourHISTORYCLIPPED, clamp(clipBlendFactor * 1.f, 0.f, 1.f));

  FragColour.rgb = mix(colourHISTORYCLIPPEDBLEND, colourCURRENT.rgb, feedback);
//  FragColour.rg = abs(vel);
//  FragColour.a = 1.f;
//  FragColour.b = 0.f;
//  FragColour.rg /= pixelSize;
}


