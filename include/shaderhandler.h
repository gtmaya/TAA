#ifndef SHADERHANDLER_H
#define SHADERHANDLER_H

#include <glm/glm.hpp>

struct ShaderProps
{
  ShaderProps(glm::vec3 _diffuseColour,
              glm::vec3 _specularColour,
              float _roughness,
              float _metallic,
              float _diffuseWeight,
              float _specularWeight);

  ~ShaderProps();
  glm::vec3 m_diffuseColour;
  glm::vec3 m_specularColour;
  float m_roughness;
  float m_metallic;
  float m_diffuseWeight;
  float m_specularWeight;
};

#endif
