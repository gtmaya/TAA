#include "usercamera.h"
#include <glm/gtx/string_cast.hpp>

UserCamera::UserCamera() : m_position (-17.543415, 5.733370, 22.232615),
                           m_rotation (4.285187, -0.210000),
                           m_velocity (0.f, 0.f, 0.f),
                           m_acceleration (0.f, 0.f, 0.f),
                           m_target (-1.f, 0.f, 0.f),
                           m_width (1),
                           m_height (1),
                           m_fovy(glm::pi<float>() * 0.25f),
                           m_aspect (float(m_width) / float(m_height)),
                           m_zNear (0.1f),
                           m_zFar (80.f),
                           m_mousePos (NULL, NULL)
{
  update();
}

void UserCamera::reset()
{
  m_position = {-17.543415, 5.733370, 22.232615};
  m_rotation = {4.285187, -0.210000};
  m_velocity = {0.f, 0.f, 0.f};
  m_acceleration = {0.f, 0.f, 0.f};
  m_target = {-1.f, 0.f, 0.f};
  m_fovy = glm::pi<float>() * 0.25f;
  m_aspect = float(m_width) / float(m_height);
  m_zNear = 0.1f;
  m_zFar = 1000.f;
  m_mousePos = {NULL, NULL};
  update();
}

UserCamera::~UserCamera() = default;

void UserCamera::handleMouseMove(const double _xpos, const double _ypos)
{
  glm::dvec2 newMousePos = glm::vec2(_xpos, _ypos);
  if (m_mousePos.x == NULL && m_mousePos.y == NULL) {m_mousePos = newMousePos;}

  glm::dvec2 deltaPos = m_mousePos - newMousePos;
  m_rotation += deltaPos * glm::dvec2(0.002);

  if (m_rotation.x > glm::pi<float>() * 2.f) {m_rotation.x = 0.f;}
  if (m_rotation.x < 0.f) {m_rotation.x = glm::pi<float>() * 2.f;}
  if (m_rotation.y < -glm::pi<float>() * 0.5f) {m_rotation.y = -glm::pi<float>() * 0.5f + 0.001f;}
  if (m_rotation.y > glm::pi<float>() * 0.5f) {m_rotation.y = glm::pi<float>() * 0.5f - 0.001f;}

  m_mousePos = newMousePos;
}

void UserCamera::handleMouseClick(const double _xpos, const double _ypos, const int _button, const int _action, const int _mods)
{}

void UserCamera::handleScroll(const double _xoffset, const double _yoffset)
{
//  m_fovy -= 0.05f * float(_yoffset);
//  if (m_fovy > glm::pi<float>() * 0.5f) {m_fovy = glm::pi<float>() * 0.5f;}
//  if (m_fovy < glm::pi<float>() * 0.01f) {m_fovy = glm::pi<float>() * 0.01f;}
}

void UserCamera::handleKey(const int _key, const int _action)
{
  bool state = false;
  switch (_action)
  {
    case (GLFW_PRESS):
    {
      state = true;
      switch (_key)
      {
        case (GLFW_KEY_LEFT):
        {
          m_keyIndex[taa_LEFT]  = !m_keyIndex[taa_LEFT];
          if(m_keyIndex[taa_LEFT])
          {
            m_keyIndex[taa_RIGHT] = !m_keyIndex[taa_LEFT];
            m_keyIndex[taa_UP]    = !m_keyIndex[taa_LEFT];
            m_keyIndex[taa_DOWN]  = !m_keyIndex[taa_LEFT];
          }
          break;
        }
        case (GLFW_KEY_RIGHT):
        {
          m_keyIndex[taa_RIGHT] = !m_keyIndex[taa_RIGHT];
          if(m_keyIndex[taa_RIGHT])
          {
            m_keyIndex[taa_LEFT]  = !m_keyIndex[taa_RIGHT];
            m_keyIndex[taa_UP]    = !m_keyIndex[taa_RIGHT];
            m_keyIndex[taa_DOWN]  = !m_keyIndex[taa_RIGHT];
          }
          break;
        }
        case (GLFW_KEY_UP):
        {
          m_keyIndex[taa_UP]  = !m_keyIndex[taa_UP];
          if(m_keyIndex[taa_UP])
          {
            m_keyIndex[taa_LEFT] = !m_keyIndex[taa_UP];
            m_keyIndex[taa_RIGHT]    = !m_keyIndex[taa_UP];
            m_keyIndex[taa_DOWN]  = !m_keyIndex[taa_UP];
          }
          break;
        }
        case (GLFW_KEY_DOWN):
        {
          m_keyIndex[taa_DOWN]  = !m_keyIndex[taa_DOWN];
          if(m_keyIndex[taa_DOWN])
          {
            m_keyIndex[taa_LEFT] = !m_keyIndex[taa_DOWN];
            m_keyIndex[taa_RIGHT]    = !m_keyIndex[taa_DOWN];
            m_keyIndex[taa_UP]  = !m_keyIndex[taa_DOWN];
          }
          break;
        }
        default : {break;}
      }
      break;
    }
    case (GLFW_REPEAT):
    {
      state = true;
      break;
    }
    default:
    {
      state = false;
      break;
    }
  }
  switch (_key)
  {
    case (GLFW_KEY_W) : {m_keyIndex[taa_W] = state; break;}
    case (GLFW_KEY_A) : {m_keyIndex[taa_A] = state; break;}
    case (GLFW_KEY_S) : {m_keyIndex[taa_S] = state; break;}
    case (GLFW_KEY_D) : {m_keyIndex[taa_D] = state; break;}
    case (GLFW_KEY_Q) : {m_keyIndex[taa_Q] = state; break;}
    case (GLFW_KEY_E) : {m_keyIndex[taa_E] = state; break;}
    default           : {break;}
  }
}

void UserCamera::resize(const int _width, const int _height)
{
  m_width = _width;
  m_height = _height;
  m_aspect = float(_width) / float(_height);
}

void UserCamera::update()
{
  if (m_keyIndex[taa_W]) {m_acceleration.x = -0.015f;}
  if (m_keyIndex[taa_S]) {m_acceleration.x =  0.015f;}
  if (m_keyIndex[taa_A]) {m_acceleration.z =  0.015f;}
  if (m_keyIndex[taa_D]) {m_acceleration.z = -0.015f;}
  if (m_keyIndex[taa_Q]) {m_acceleration.y = -0.015f;}
  if (m_keyIndex[taa_E]) {m_acceleration.y =  0.015f;}

  if (m_keyIndex[taa_LEFT])  {m_position.z +=  0.1f;}
  if (m_keyIndex[taa_RIGHT]) {m_position.z -=  0.1f;}
  if (m_keyIndex[taa_UP])    {m_position.y +=  0.1f;}
  if (m_keyIndex[taa_DOWN])  {m_position.y -=  0.1f;}

  if (!m_keyIndex[taa_W] && !m_keyIndex[taa_S]) {m_acceleration.x = 0.f;}
  if (!m_keyIndex[taa_A] && !m_keyIndex[taa_D]) {m_acceleration.z = 0.f;}
  if (!m_keyIndex[taa_Q] && !m_keyIndex[taa_E]) {m_acceleration.y = 0.f;}

  m_acceleration = glm::rotate(m_acceleration, -m_rotation.y, glm::vec3(0.f, 0.f, 1.f));
  m_acceleration = glm::rotate(m_acceleration, m_rotation.x, glm::vec3(0.f, 1.f, 0.f));


  m_velocity += m_acceleration;
  m_velocity *= 0.85f;

  m_position += m_velocity;
  m_target = glm::vec3(-1.f, 0.f, 0.f); //Default target is one unit in front of the camera at the origin.


  m_target = glm::rotate(m_target, -m_rotation.y, glm::vec3(0.f, 0.f, 1.f));
  m_target = glm::rotate(m_target, m_rotation.x, glm::vec3(0.f, 1.f, 0.f));
  m_target += m_position;
  m_view = glm::lookAt(m_position, glm::vec3(m_target), glm::vec3(0.0f,1.0f,0.0f));
  m_proj = glm::perspective(m_fovy, m_aspect, m_zNear, m_zFar);
  m_cube = glm::lookAt({0.f, 0.f, 0.f}, m_target - m_position, {0.f, 1.f, 0.f});


  std::cout<<glm::to_string(m_position)<<'\n'<<glm::to_string(m_rotation)<<'\n';
}

glm::mat4 UserCamera::viewMatrix() const
{
  return m_view;
}

glm::mat4 UserCamera::projMatrix() const
{
  return m_proj;
}

glm::mat4 UserCamera::cubeMatrix() const
{
  return m_cube;
}

glm::vec3 UserCamera::getLocation() const
{
  return m_position;
}

void UserCamera::toggleCursorState()
{
  m_trackingActive = !m_trackingActive;
}

bool UserCamera::cursorActive() const
{
  return m_trackingActive;
}
