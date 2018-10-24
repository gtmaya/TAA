#include "usercamera.h"

UserCamera::UserCamera() : m_position (10.f, 0.f, 2.f),
                           m_rotation (0.f, 0.f),
                           m_velocity (0.f, 0.f, 0.f),
                           m_accelleration (0.f, 0.f, 0.f),
                           m_target (-1.f, 0.f, 0.f),
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
  std::cout<<"ROT = "<<m_rotation[0]<<','<<m_rotation[1]<<'\n';
  //std::cout<<"Mouse pos = "<<_xpos<<" , "<<_ypos<<'\n';
  glm::dvec2 newPos = {_xpos, _ypos};

  static bool firstInit = true;
  if (firstInit)
  {
    m_mousePos = newPos;
    firstInit = false;
  }

  glm::dvec2 deltaPos = m_mousePos - newPos;
  //std::cout<<"DeltaPos = "<<deltaPos[0]<<", "<<deltaPos[1]<<'\n';
  m_mousePos = newPos;

  m_rotation += glm::dvec2(0.1f, 0.1f) * deltaPos;
  m_camMoved = true;
  std::cout<<"ROT = "<<m_rotation[0]<<','<<m_rotation[1]<<'\n';
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
  m_velocity += m_accelleration;
  m_position += m_velocity;
  glm::vec3 target = glm::vec3(-1.f, 0.f, 0.f);
  target = glm::rotateY(target, glm::radians(m_rotation[0]));
  target = glm::rotateZ(target, glm::radians(-m_rotation[1]));
  m_target = m_position + target;
  //std::cout<<m_target[0]<<','<<m_target[1]<<','<<m_target[2]<<'\n';
  m_view = glm::lookAt(m_position, glm::vec3(m_target), glm::vec3(0.0f,1.0f,0.0f));
  m_proj = glm::perspective(m_fovy, m_aspect, m_zNear, m_zFar);
}

glm::mat4 UserCamera::viewMatrix() const
{
  return m_view;
}

glm::mat4 UserCamera::projMatrix() const
{
  return m_proj;
}

