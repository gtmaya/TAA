#version 430

//FragColor
layout (location=0) out vec4 FragColor;

//Light info
uniform vec3 lightPos[14];
uniform vec3 lightCol[14];

//Textures
uniform samplerCube envMap;
uniform int envMapMaxLod;

//Shader parameters
uniform float roughness;
uniform float metallic;
uniform float diffAmount;
uniform float specAmount;
uniform vec3 materialDiff;
uniform vec3 materialSpec;
uniform float alpha;
uniform sampler2D diffuseMap;
uniform bool hasDiffMap;
uniform vec3 cameraPos;
uniform mat4 MV;

// Vectors from the vertex shader.
in vec3 fragNormal;
in vec4 fragPosition;
in vec2 fragTexCoord;
in vec3 worldPos;
in vec3 viewSpacePos;

//Constants
const float small = 0.0001f;
const float pi = 3.1415926f;

vec3 specularComponent(vec3 _n, vec3 _v, vec3 _s, float _roughness, vec3 _fInc)
{
  float m = _roughness;
  float mSquared = pow(m, 2);

  //Beckmannn Distribution
  vec3 h = normalize(_v + _s);
  float NdotH = dot(_n, h);
  float VdotH = dot(_v, h);
  float NdotV = dot(_n, _v);
  float NdotL = dot(_n, _s);
  float r1  = 1.f / (4.f * mSquared * pow(NdotH, 4.f));
  float r2 = (NdotH * NdotH - 1.f) / (mSquared * NdotH * NdotH);
  float D = r1 * exp(r2);

  float denom = NdotH * NdotH * (mSquared - 1.f) + 1.f;
  D = mSquared / (pi * denom * denom);

  // Geometric attenuation
  float NH2 = 2.f * NdotH;
  float invVdotH = (VdotH > small)?(1.f / VdotH):1.f;
  float g1 = (NH2 * NdotV) * invVdotH;
  float g2 = (NH2 * NdotL) * invVdotH;
  float G = min(1.f, min(g1, g2));

  // Schlick approximation
  float F0 = 0.2; // Fresnel reflectance at normal incidence
  float F_r = pow(1.0 - VdotH, 5.0) * (1.0 - _fInc.r) + _fInc.r;
  float F_g = pow(1.0 - VdotH, 5.0) * (1.0 - _fInc.g) + _fInc.g;
  float F_b = pow(1.0 - VdotH, 5.0) * (1.0 - _fInc.b) + _fInc.b;
  vec3 F = vec3(F_r, F_g, F_b);

  // Compute the light from the ambient, diffuse and specular components
  return F * D /*/ NdotV*/;
}

void main()
{
  vec3 n = normalize(fragNormal);
  vec3 v = vec3(0.0f, 0.0f, 1.0f);
  v = normalize(cameraPos - worldPos);

  vec3 p = fragPosition.xyz / fragPosition.w;
  vec3 lookup = (reflect(-v,n));
  //lookup.z *= -1;
  lookup.y *= -1;

  vec3 totalLight = vec3(0.f);

  vec3 materialDiffuseMap;
  if (hasDiffMap)
  {
    materialDiffuseMap = texture(diffuseMap, fragTexCoord).rgb;
  }
  else
  {
    materialDiffuseMap = vec3(1.f);
  }

  for(int i = 0; i < 14; i++)
  {
    vec3 transformedLightPos = lightPos[i] * 100.f;
    float distance = length(transformedLightPos - worldPos.xyz);
    float attenuation = 1.0 / (distance * distance);
    vec3 s = normalize(transformedLightPos - worldPos.xyz);

    vec3 specComponent = specularComponent(n, v, s, roughness, vec3(metallic, metallic, metallic));
    specComponent = max(min(specComponent, 1), 0);
    vec3 specularIntensity = textureLod(envMap, lookup, roughness * envMapMaxLod).rgb;

    vec3 diffuseIntensity = lightCol[i] * max(dot(s, n), 0.0);
    totalLight += vec3((diffuseIntensity * diffAmount * materialDiff * materialDiffuseMap) +
                   (specularIntensity * specComponent * specAmount * materialSpec));
  }
  FragColor = vec4(totalLight , alpha);
}
