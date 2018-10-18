#include "usercamera.h"

UserCamera::UserCamera() : m_position (10.f, 0.f, 0.f),
                           m_rotation (0.f, 0.f, 0.f),
                           m_velocity (0.f, 0.f, 0.f),
                           m_accelleration (0.f, 0.f, 0.f),
                           m_target (0.f, 0.f, 0.f),
                           m_width (1),
                           m_height (1),
                           m_fovy(glm::pi<float>() * 0.25f),
                           m_aspect (float(m_width) / float(m_height)),
                           m_zNear (0.1f),
                           m_zFar (100.f)
{
  update();
}

void UserCamera::handleMouseMove(const double _xpos, const double _ypos)
{
  std::cout<<"Mouse pos = "<<_xpos<<" , "<<_ypos<<'\n';
}

void UserCamera::handleMouseClick(const double _xpos, const double _ypos, const int _button, const int _action, const int _mods)
{
  std::cout<<"Mouse pos = "<<_xpos<<", "<<_ypos<<"; button = "<<_button<<" action = "<<_action<<" mods = "<<_mods<<'\n';
}

void UserCamera::handleKey(const int _key, const bool _state)
{
  std::cout<<"Key = "<<_key<<'\n';
  if (_key == 87)
  {
    //std::cout<<"SHOULD BE MOVING OR SUMMIN'\n";
    m_velocity -= glm::vec3(0.01f, 0.f, 0.f);
  }
  if (_key == 83)
  {
    m_velocity += glm::vec3(0.01f, 0.f, 0.f);
  }
  if (_key == 65)
  {
    //std::cout<<"SHOULD BE MOVING OR SUMMIN'\n";
    m_velocity += glm::vec3(0.f, 0.f, 0.01f);
  }
  if (_key == 68)
  {
    m_velocity -= glm::vec3(0.f, 0.f, 0.01f);
  }
  if (_key == 69)
  {
    m_velocity += glm::vec3(0.f, 0.01f, 0.f);
  }
  if (_key == 81)
  {
    m_velocity -= glm::vec3(0.f, 0.01f, 0.f);
  }
}

void UserCamera::resize(const int _width, const int _height)
{
  m_width = _width;
  m_height = _height;
}

void UserCamera::update()
{
  m_velocity += m_accelleration;
  m_position += m_velocity;
  m_target = m_position + glm::vec3(-1.f, 0.f, 0.f);
  m_view = glm::lookAt(m_position, glm::vec3(m_target), glm::vec3(0.0f,1.0f,0.0f));
  m_proj = glm::perspective(m_fovy, m_aspect, m_zNear, m_zFar);
  //std::cout<<"m_position = "<<m_position.x<<'\n';
}

glm::mat4 UserCamera::viewMatrix() const
{
  return m_view;
}

glm::mat4 UserCamera::projMatrix() const
{
  return m_proj;
}

