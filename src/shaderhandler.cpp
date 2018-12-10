#include "shaderhandler.h"

ShaderProps::ShaderProps(glm::vec3 _diffuseColour,
                         glm::vec3 _specularColour,
                         float _roughness,
                         float _metallic,
                         float _diffuseWeight,
                         float _specularWeight,
                         float _alpha,
                         int _texture)
                         :
                         m_diffuseColour (_diffuseColour),
                         m_specularColour (_specularColour),
                         m_roughness (_roughness),
                         m_metallic (_metallic),
                         m_diffuseWeight (_diffuseWeight),
                         m_specularWeight (_specularWeight),
                         m_alpha (_alpha),
                         m_diffuseTex (_texture)

{}

ShaderProps::~ShaderProps() = default;
