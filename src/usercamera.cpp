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
/// @file usercamera.cpp
/// @brief A first person camera.
//---------------------------------------------------------------------------------------------------------------
#include "usercamera.h"
#include <glm/gtx/string_cast.hpp>
//---------------------------------------------------------------------------------------------------------------
/// @brief Constructor for default camera properties.
//---------------------------------------------------------------------------------------------------------------
UserCamera::UserCamera() : m_position (100.f, 0.f, 0.f),
                           m_rotation (0.f, 0.f),
                           m_velocity (0.f, 0.f, 0.f),
                           m_acceleration (0.f, 0.f, 0.f),
                           m_target (-1.f, 0.f, 0.f),
                           m_mousePos (NULL, NULL),
                           m_width (1),
                           m_height (1),
                           m_fovy(glm::pi<float>() * 0.25f),
                           m_aspect (float(m_width) / float(m_height)),
                           m_zNear (0.5f),
                           m_zFar (300.f)
{
  update();
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Default destructor.
//---------------------------------------------------------------------------------------------------------------
UserCamera::~UserCamera() = default;
//---------------------------------------------------------------------------------------------------------------
/// @brief Function called to reset the camera's properties to their values at the beginning of the program.
/// Called when enter is pressed.
//---------------------------------------------------------------------------------------------------------------
void UserCamera::reset()
{
  m_position = {100.f, 0.f, 0.f};
  m_rotation = {0.f, 0.f};
  m_velocity = {0.f, 0.f, 0.f};
  m_acceleration = {0.f, 0.f, 0.f};
  m_target = {-1.f, 0.f, 0.f};
  m_fovy = glm::pi<float>() * 0.25f;
  m_aspect = float(m_width) / float(m_height);
  m_zNear = 0.5f;
  m_zFar = 300.f;
  m_mousePos = {NULL, NULL};
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Function that finds the 2D displacement of the mouse in the window and calculates the camera's
/// rotation dependant on this displacement.
/// @param _xpos The current X position of the mouse
/// @param _ypos The current Y position of the mouse
//---------------------------------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------------------------------
/// @brief As yet unused function that would handle mouse clicks.
/// @param window Window handle (unused currently)
/// @param button Which button was pressed (e.g. left or right button)
/// @param action GLFW code for the action (GLFW_PRESS or GLFW_RELEASE)
/// @param mods Other keys which are currently being held down (e.g. GLFW_CTRL)
//---------------------------------------------------------------------------------------------------------------
void UserCamera::handleMouseClick(const double _xpos, const double _ypos, const int _button, const int _action, const int _mods)
{}
//---------------------------------------------------------------------------------------------------------------
/// @brief Function that handles scroll wheel movement to change the FOV of the camera, clamping it between
/// pi/100 radians (1.8 degrees) and ~2pi/3 radians (~120 degrees)
/// @param xoffset How far has been scrolled in the x dimension
/// @param yoffset How far has been scrolled in the y dimension
//---------------------------------------------------------------------------------------------------------------
void UserCamera::handleScroll(const double _xoffset, const double _yoffset)
{
  m_fovy -= 0.05f * float(_yoffset);
  if (m_fovy > glm::pi<float>() * 0.667f) {m_fovy = glm::pi<float>() * 0.667f;}
  if (m_fovy < glm::pi<float>() * 0.01f) {m_fovy = glm::pi<float>() * 0.01f;}
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Function that handles key input to the camera, allowing for multiple keys to be pressed at once,
/// ignoring key repeats from the OS.
/// @param key The key that was pressed
/// @param action GLFW code for the action (GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT)
//---------------------------------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------------------------------
/// @brief Function to update the camera with the width and height of the scene so the view frustrum can be sized
/// correctly.
/// @param _width Width input
/// @param _height Height input
//---------------------------------------------------------------------------------------------------------------
void UserCamera::resize(const int _width, const int _height)
{
  m_width = _width;
  m_height = _height;
  m_aspect = float(_width) / float(_height);
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Function to update the camera's target (and thus its orientation) and location, in a frame rate
/// independant way.
//---------------------------------------------------------------------------------------------------------------
void UserCamera::update()
{
  auto now = std::chrono::high_resolution_clock::now();
  static auto last = std::chrono::high_resolution_clock::now();
  float delta = std::chrono::duration_cast<std::chrono::nanoseconds>(now - last).count();
  last = now;
  delta *= 0.000000001f;

  if (m_keyIndex[taa_W]) {m_acceleration.x = -1.25f;}
  if (m_keyIndex[taa_S]) {m_acceleration.x =  1.25f;}
  if (m_keyIndex[taa_A]) {m_acceleration.z =  1.25f;}
  if (m_keyIndex[taa_D]) {m_acceleration.z = -1.25f;}
  if (m_keyIndex[taa_Q]) {m_acceleration.y = -1.25f;}
  if (m_keyIndex[taa_E]) {m_acceleration.y =  1.25f;}

  if (m_keyIndex[taa_LEFT])  {m_position.z +=  10.f * delta;}
  if (m_keyIndex[taa_RIGHT]) {m_position.z -=  10.f * delta;}
  if (m_keyIndex[taa_UP])    {m_position.y +=  10.f * delta;}
  if (m_keyIndex[taa_DOWN])  {m_position.y -=  10.f * delta;}

  if (!m_keyIndex[taa_W] && !m_keyIndex[taa_S]) {m_acceleration.x = 0.f;}
  if (!m_keyIndex[taa_A] && !m_keyIndex[taa_D]) {m_acceleration.z = 0.f;}
  if (!m_keyIndex[taa_Q] && !m_keyIndex[taa_E]) {m_acceleration.y = 0.f;}

  m_acceleration = glm::rotate(m_acceleration, -m_rotation.y, glm::vec3(0.f, 0.f, 1.f));
  m_acceleration = glm::rotate(m_acceleration, m_rotation.x, glm::vec3(0.f, 1.f, 0.f));

  m_velocity += m_acceleration * delta;
  m_velocity *= 0.85f;
  m_position += m_velocity;
  m_target = glm::vec3(-1.f, 0.f, 0.f); //Default target is one unit in front of the camera at the origin.

  m_target = glm::rotate(m_target, -m_rotation.y, glm::vec3(0.f, 0.f, 1.f));
  m_target = glm::rotate(m_target, m_rotation.x, glm::vec3(0.f, 1.f, 0.f));
  m_target += m_position;
  m_view = glm::lookAt(m_position, glm::vec3(m_target), glm::vec3(0.0f,1.0f,0.0f));
  m_proj = glm::perspective(m_fovy, m_aspect, m_zNear, m_zFar);
  m_cube = glm::lookAt({0.f, 0.f, 0.f}, m_target - m_position, {0.f, 1.f, 0.f});
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Function to pass the view matrix to the scene.
//---------------------------------------------------------------------------------------------------------------
glm::mat4 UserCamera::viewMatrix() const
{
  return m_view;
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Function to pass the projection matrix to the scene.
//---------------------------------------------------------------------------------------------------------------
glm::mat4 UserCamera::projMatrix() const
{
  return m_proj;
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Function to pass the environment cube view matrix (i.e. one that has the camera aiming at the sides of
/// the cube while remaining in the centre) to the scene.
//---------------------------------------------------------------------------------------------------------------
glm::mat4 UserCamera::cubeMatrix() const
{
  return m_cube;
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Function to pass the location to the scene.
//---------------------------------------------------------------------------------------------------------------
glm::vec3 UserCamera::getLocation() const
{
  return m_position;
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Function to toggle whether the camera should track the cursor for rotation or not.
//---------------------------------------------------------------------------------------------------------------
void UserCamera::toggleCursorState()
{
  m_trackingActive = !m_trackingActive;
  if (!m_trackingActive) {m_mousePos = {NULL, NULL};}
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Function returning the cursor state for use elsewhere.
//---------------------------------------------------------------------------------------------------------------
bool UserCamera::cursorActive() const
{
  return m_trackingActive;
}
