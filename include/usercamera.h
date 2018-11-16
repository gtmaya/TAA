#ifndef USERCAMERA_H
#define USERCAMERA_H

#include <ngl/Obj.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <GLFW/glfw3.h>

class UserCamera
{
  public:
    UserCamera();
    ~UserCamera();
    void handleMouseMove(const double _xpos, const double _ypos);
    void handleMouseClick(const double _xpos, const double _ypos, const int _button, const int _action, const int _mods);
    void handleScroll(const double _xoffset, const double _yoffset);
    void handleKey(const int _key, const int _action);
    void resize(const int _width, const int _height);
    void update();
    glm::mat4 viewMatrix() const;
    glm::mat4 projMatrix() const;
    glm::mat4 cubeMatrix() const;
    glm::vec3 getLocation() const;
    void toggleCursorState();
    bool cursorActive() const;
    void reset();


  private:
    glm::vec3 m_position;
    glm::vec2 m_rotation;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;
    glm::vec3 m_target;
    int m_width;
    int m_height;
    glm::mat4 m_view;
    glm::mat4 m_proj;
    glm::mat4 m_cube;
    float m_fovy;
    float m_aspect;
    float m_zNear;
    float m_zFar;
    std::array<bool, 4> m_keyIndex;
    enum m_taaKeyIndex {taa_W, taa_A, taa_S, taa_D, taa_Q, taa_E, taa_SPACE, taa_ESC, taa_LEFT, taa_RIGHT, taa_UP, taa_DOWN};
    glm::dvec2 m_mousePos;
    bool m_camMoved = true;
    bool m_trackingActive = true;
};

#endif
