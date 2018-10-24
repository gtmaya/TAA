#include "usercamera.h"
#include <glm/gtx/string_cast.hpp>

UserCamera::UserCamera() : m_position (0.f, 0.f, 0.f),
                           m_rotation (0.f, 0.2f),
                           m_velocity (0.f, 0.f, 0.f),
                           m_accelleration (0.f, 0.f, 0.f),
                           m_target (-1.f, 0.f, 0.f),
                           m_width (1),
                           m_height (1),
                           m_fovy(glm::pi<float>() * 0.25f),
                           m_aspect (float(m_width) / float(m_height)),
                           m_zNear (0.1f),
                           m_zFar (100.f),
                           m_mousePos (NULL, NULL)
{
  update();
}

void UserCamera::handleMouseMove(const double _xpos, const double _ypos)
{
  glm::dvec2 newMousePos = glm::vec2(_xpos, _ypos);
  if (m_mousePos.x == NULL && m_mousePos.y == NULL) {m_mousePos = newMousePos;}
  glm::dvec2 deltaPos = m_mousePos - newMousePos;
  std::cout<<"deltaPos = "<<glm::to_string(deltaPos)<<'\n';

  m_rotation += deltaPos * glm::dvec2(0.002);

  std::cout<<"Rotation = "<<glm::to_string(m_rotation)<<'\n';
  m_mousePos = newMousePos;
}

void UserCamera::handleMouseClick(const double _xpos, const double _ypos, const int _button, const int _action, const int _mods)
{
  //std::cout<<"Mouse pos = "<<_xpos<<", "<<_ypos<<"; button = "<<_button<<" action = "<<_action<<" mods = "<<_mods<<'\n';
}

void UserCamera::handleKey(const int _key, const bool _state)
{
  static bool wActuallyDown = false;
  if (m_keyIndex[taa_W])
  {
    //If state changed last frame, W was either pressed or released from a long state of hold
    wActuallyDown = !wActuallyDown;
  }

  switch (_key)
  {
    case 'W':
    {
      m_keyIndex[taa_W] = _state;
      std::cout<<m_keyIndex[taa_W]<<'\n';  /*Noticed that if W is held, this returns true only for the initial press and then for the release.
                                             if W is pressed and released quickly however it returns true only for the intial update.*/
      if (wActuallyDown){std::cout<<"ACUTALLY DOWN\n";} /*This works if W is held, reporting the key is down and ignoring the key repeats, however does not work with single presses
                                                          as a press essentially causes wActuallyDown to flip flop*/
      break;
    }
    case 'A':
      m_keyIndex[taa_A]= _state;
      break;
    case 'S':
      m_keyIndex[taa_S] = _state;
      break;
    case 'D':
      m_keyIndex[taa_D] = _state;
      break;
    default:
      break;
  }
}

void UserCamera::resize(const int _width, const int _height)
{
  m_width = _width;
  m_height = _height;
}

void UserCamera::update()
{

  //handleMouseMove(m_mousePos.x + 5.f, m_mousePos.y);


  if (m_rotation.x > glm::pi<float>() * 2.f) {m_rotation.x = 0.f;}
  if (m_rotation.x < 0.f) {m_rotation.x = glm::pi<float>() * 2.f;}

  if (m_rotation.y < -glm::pi<float>() * 0.5f) {m_rotation.y = -glm::pi<float>() * 0.5f + 0.001f;}
  if (m_rotation.y > glm::pi<float>() * 0.5f) {m_rotation.y = glm::pi<float>() * 0.5f - 0.001f;}

//  m_rotation.y -= 0.01f;
//  m_rotation.x += 0.01f;


  m_velocity += m_accelleration;
  m_position += m_velocity;
  m_target = glm::vec3(-1.f, 0.f, 0.f); //Default target is one unit in front of the camera at the origin.


  m_target = glm::rotate(m_target, -m_rotation.y, glm::vec3(0.f, 0.f, 1.f));
  m_target = glm::rotate(m_target, m_rotation.x, glm::vec3(0.f, 1.f, 0.f));
  m_view = glm::lookAt(m_position, glm::vec3(m_target), glm::vec3(0.0f,1.0f,0.0f));
  m_proj = glm::perspective(m_fovy, m_aspect, m_zNear, m_zFar);

  //m_rotation = glm::vec2(0.f);
}

glm::mat4 UserCamera::viewMatrix() const
{
  return m_view;
}

glm::mat4 UserCamera::projMatrix() const
{
  return m_proj;
}

