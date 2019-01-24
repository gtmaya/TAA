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
/// @file shaderhandler.h
/// @brief Handles and stores shader properties.
//---------------------------------------------------------------------------------------------------------------
#ifndef SHADERHANDLER_H
#define SHADERHANDLER_H

#include <ngl/Types.h>
#include <glm/glm.hpp>

struct ShaderProps
{
  ShaderProps(glm::vec3 _diffuseColour,
              glm::vec3 _specularColour,
              float _roughness,
              float _metallic,
              float _diffuseWeight,
              float _specularWeight,
              float _alpha,
              int _texture);

  ~ShaderProps();
  glm::vec3 m_diffuseColour;
  glm::vec3 m_specularColour;
  float m_roughness;
  float m_metallic;
  float m_diffuseWeight;
  float m_specularWeight;
  float m_alpha;
  GLuint m_diffuseTex;
};

#endif
