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
/// @file taa_f.glsl
/// @brief The main TAA routine (i.e. where the bulk of the processing is happening for TAA to work)
//---------------------------------------------------------------------------------------------------------------
#version 430

//Inverse VP for the current frame to unproject the fragment
uniform mat4 inverseViewProjectionCURRENT;
//VP for the previous frame to then reproject the fragment to it's previous position.
uniform mat4 viewProjectionHISTORY;
//The three incoming textures from the current frame
uniform sampler2D colourRENDER;
uniform sampler2D velocityBUF;
uniform sampler2D depthRENDER;
//The input result from the previous TAA pass
uniform sampler2D colourANTIALIASED;
//Variables needed so that we're sampling the textures at the correct scale in each dimension.
uniform vec2 windowSize;
uniform vec2 pixelSize;
//Jitter so that we can unjitter the incoming rendered textures.
uniform vec2 jitter;
//How much of each frame we are keeping, this is controlled by the user.
uniform float feedback;

//There is only one output
layout (location=0) out vec4 FragColour;

//From the vertex shader
in vec2 FragmentUV;

//Matrices to transform the colour space for neighbourhood clipping
uniform mat3 YCoCGMatrix = mat3(0.25f, 0.5f, -0.25f, 0.5f, 0.f, 0.5f, 0.25f, -0.5f, -0.25f);
uniform mat3 RGBMatrix = mat3(1.f, 1.f, 1.f, 1.f, 0.f, -1.f, -1.f, 1.f, -1.f);

//Flag to be set when a fragment is clipped so that we can reduce flickering slightly
bool clipped = false;

/// @brief Function to transform RGB colour to YCoCg space.
/// @param _inRGB The RGB input
vec3 YCoCg(vec3 _inRGB)
{
  return _inRGB;
  return YCoCGMatrix * _inRGB;
}

/// @brief Function to transform YCoCg colour to RGB space.
/// @param _inYCoCg The YCoCg input
vec3 RGB(vec3 _inYCoCG)
{
  return _inYCoCG;
  return RGBMatrix * _inYCoCG;
}

/// @brief Function to sample the rendered texture and convert it to YCoCg (makes code more readable later)
/// @param _UV the coordinate we are sampling.
vec3 sampleRenderYCoCg(vec2 _UV)
{
  return YCoCg(texture(colourRENDER, _UV).rgb);
}

/// @brief Function to return the minimum YCoCg value of the current pixel and its 8 neighbours
/// @param _uvCURRENT the coordinate we are sampling.
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

/// @brief Function to return the minimum YCoCg value of the current pixel and its 4 direct neighbours
/// @param _uvCURRENT the coordinate we are sampling.
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

/// @brief Function to return the maximum YCoCg value of the current pixel and its 8 neighbours
/// @param _uvCURRENT the coordinate we are sampling.
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

/// @brief Function to return the minimum YCoCg value of the current pixel and its 4 direct neighbours
/// @param _uvCURRENT the coordinate we are sampling.
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

/// @brief Function to determine if the current fragment's history sample lies in or outside the bounding box of its neighours' chrominance values in the current frame.  If it does, it clips the colour against this bounding box, otherwise it returns the sample.
/// @param _colourSample the current fragment's history sample.
/// @param _uvCURRENT the coordinate we are sampling.
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
    return RGB(clippedColour);
  }
  else {return RGB(YCoCgSample);}
}

/// @brief Function to determine the coordinates in screen space of the neighbouring fragment with the closest depth value.  This allows us to `dilate' the velocity buffer when sampling it, as it is aliased.
/// @param _coord The coordinate of the fragment we are processing.
vec2 frontMostNeigbourCoord(vec2 _coord)
{
  float samp[9];
  samp[0] = texture(depthRENDER, _coord).r;
  samp[1] = texture(depthRENDER, _coord + vec2( pixelSize.x, 0.f)).r;
  samp[2] = texture(depthRENDER, _coord + vec2(-pixelSize.x, 0.f)).r;
  samp[3] = texture(depthRENDER, _coord + vec2(0.f,  pixelSize.y)).r;
  samp[4] = texture(depthRENDER, _coord + vec2(0.f, -pixelSize.y)).r;
  samp[5] = texture(depthRENDER, _coord + vec2( pixelSize.x,  pixelSize.y)).r;
  samp[6] = texture(depthRENDER, _coord + vec2(-pixelSize.x,  pixelSize.y)).r;
  samp[7] = texture(depthRENDER, _coord + vec2( pixelSize.x, -pixelSize.y)).r;
  samp[8] = texture(depthRENDER, _coord + vec2(-pixelSize.x, -pixelSize.y)).r;

  int neighbour = 0;
  float minSamp = samp[0];
  for (int i = 0; i < 9; i++)
  {
    if (samp[i] < minSamp) {minSamp = samp[i]; neighbour = i;}
  }
  //Switch statement to avoid a horrible if-else mess.
  switch (neighbour)
  {
    case 0:
      return vec2(0.f, 0.f) + _coord;
    case 1:
      return vec2(pixelSize.x, 0.f) + _coord;
    case 2:
      return vec2(-pixelSize.x, 0.f) + _coord;
    case 3:
      return vec2(0.f, pixelSize.y) + _coord;
    case 4:
      return vec2(0.f, -pixelSize.y) + _coord;
    case 5:
      return vec2(pixelSize.x, pixelSize.y) + _coord;
    case 6:
      return vec2(-pixelSize.x, pixelSize.y) + _coord;
    case 7:
      return vec2(pixelSize.x, -pixelSize.y) + _coord;
    case 8:
      return vec2(-pixelSize.x, -pixelSize.y) + _coord;
  }
}

void main()
{
  //Find the 'UV' coordinates of the current fragment.
  vec2 uvCURRENT = gl_FragCoord.xy / windowSize;

  //Get current frame data
  vec4 colourCURRENT = texture(colourRENDER, uvCURRENT - jitter);
  float depthCURRENT = texture(depthRENDER, uvCURRENT - jitter).r;

  //Convert current screenspace to world space
  float z = depthCURRENT * 2.0 - 1.0;
  vec4 CVVPosCURRENT = vec4((uvCURRENT) * 2.f - 1.f, z, 1.f);
  vec4 worldSpacePosition = inverseViewProjectionCURRENT * CVVPosCURRENT;
  worldSpacePosition /= worldSpacePosition.w;

  //Convert this into previous UV coords.
  vec4 CVVPosHISTORY = viewProjectionHISTORY * worldSpacePosition;
  vec2 uvHISTORY = 0.5 * (CVVPosHISTORY.xy / CVVPosHISTORY.w) + 0.5;

  //Initialise the velocity to account for the jitter
  vec2 vel = uvHISTORY - uvCURRENT;
  //Add on the vector that maps the fragment's current position to it's position last frame in unjittered space as it may be dynamic.
  vel += texture(velocityBUF, frontMostNeigbourCoord(uvCURRENT - jitter)).rg * pixelSize;
  //The previous UV coords are therefore the current ones with this velocity tacked on.
  uvHISTORY = uvCURRENT + vel;

  //Get previous frame colour
  vec4 colourHISTORY = texture(colourANTIALIASED, vec2(uvHISTORY));

  //Clip it
  vec3 colourHISTORYCLIPPED = clipNeighbourhood(colourHISTORY.rgb, uvCURRENT);

  if (colourHISTORY.a == 0.f) {FragColour.a = float(clipped);} //If there's nothing, store the clipped flag (could still be nothing)
  else {FragColour.a = mix(colourHISTORY.a, float(clipped), feedback);} //If there is something, blend the previous clipped value with the current one (using same feedback as rest of AA)

  //This just makes the next line easier to read
  float clipBlendFactor = FragColour.a;
  //Lerp based on recent clipping events
  vec3 colourHISTORYCLIPPEDBLEND = mix(colourHISTORY.rgb, colourHISTORYCLIPPED, clamp(clipBlendFactor, 0.f, 1.f));
  //Now we have our two colour values, lerp between them based on the feedback factor.
  FragColour.rgb = mix(colourHISTORYCLIPPEDBLEND, colourCURRENT.rgb, feedback);
}


